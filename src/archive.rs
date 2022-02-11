
use sha2::Sha256;
use sha2::Digest;
use std::fs::File;
use std::io::copy;
use std::io::Write;

use std::error::Error;

#[derive(Debug,Clone)]
pub enum ArchiveEntry {
    FileEntry {name: String, checksum: [u8;32], file_length: u64},
    DirEntry(ArchiveHeader),
}

#[derive(Debug)]
pub enum ArchiveError {
}

impl std::fmt::Display for ArchiveError {
    fn fmt(&self, _fmt: &mut std::fmt::Formatter)  -> Result<(), std::fmt::Error> {
        Ok(())
    }
}

impl Error for ArchiveError {

}

impl ArchiveEntry {
    pub fn new_file(name: &str) -> Result<Self, Box<dyn Error>> {
        let mut file = File::open(&name)?;
        let mut hash = Sha256::new();
        copy(&mut file, &mut hash)?;

        let file_length = file.metadata()?.len();

        Ok(ArchiveEntry::FileEntry { name: String::from(name), checksum: hash.finalize().into(), file_length })
    }

    pub fn new_dir(name: &str) -> Result<Self, Box<dyn Error>> {
        let mut dir = ArchiveHeader { name: name.into(), entries: vec![], length: 0 };

        for f in std::fs::read_dir(name)? {
            match f?.path().to_str() {
                Some(path) if 
                    path.ends_with("/..") || path.ends_with("/.") || path == ".." || path == "." => {},
                Some(path) => { 
                    dir.entries.push(ArchiveEntry::new(path)?)},
                None => {},
            }
        }

        Ok(Self::DirEntry(dir))
    }

    pub fn new(name: &str) -> Result<Self, Box<dyn Error>> {
        if std::fs::metadata(name)?.is_dir() {
            Self::new_dir(name)
        } else {
            Self::new_file(name)
        }
    }
}

#[derive(Debug,Clone)]
pub struct ArchiveHeader {
    pub name: String,
    pub entries: Vec<ArchiveEntry>,
    pub length: u64,
}

pub trait ToBytes {
    fn to_le_bytes(self) -> Box<[u8]>;
    fn to_be_bytes(self) -> Box<[u8]>;
}

impl ToBytes for u64 {
    fn to_le_bytes(self) -> Box<[u8]> {
        let mut bytes = u64::to_le_bytes(self);
        bytes.reverse();

        Box::new(bytes)
    }

    fn to_be_bytes(self) -> Box<[u8]> {
        Box::new(u64::to_be_bytes(self))
    }
}

impl ToBytes for u8 {
    fn to_le_bytes(self) -> Box<[u8]> {
        Box::new(u8::to_le_bytes(self))
    }

    fn to_be_bytes(self) -> Box<[u8]> {
        Box::new(u8::to_be_bytes(self))
    }

}

impl ArchiveHeader {

    pub fn output_to<T: Write>(&mut self, file: &mut T) -> Result<(), Box<dyn Error>> {
        file.write(&(self.name.len() as u64).to_le_bytes())?;
        file.write(self.name.as_bytes())?;
        file.write(&(self.entries.len() as u64).to_le_bytes())?;

        for e in self.entries.iter() {
            match e.clone() {
                ArchiveEntry::FileEntry {name, checksum, file_length} => {
                    let mut data: Vec<u8> = vec![];
                    data.write(&(false as u8).to_le_bytes())?;
                    data.write(&(file_length as u64).to_le_bytes())?;
                    data.write(&checksum)?;
                    data.write(&(name.len() as u64).to_le_bytes())?;
                    data.write(name.as_bytes())?;

                    file.write(&((data.len() + 8) as u64).to_le_bytes())?;
                    file.write(&(*data))?;
                },
                ArchiveEntry::DirEntry(entry) => {
                    let mut data: Vec<u8> = vec![];
                    data.write(&(true as u8).to_le_bytes())?;
                    data.write(&entry.length.to_le_bytes())?;
                },
            }
        }

        Ok(())
    }

}
