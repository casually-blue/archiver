
use sha2::Sha256;
use sha2::Digest;
use std::fs::File;
use std::io::copy;
use std::io::Write;
use std::io::Read;

use std::error::Error;

#[derive(Debug,Clone)]
pub enum ArchiveEntry {
    FileEntry {name: String, checksum: [u8;32], file_length: u64},
    DirEntry(ArchiveHeader),
}

impl ArchiveEntry {
    pub fn length(&self) -> u64 {
        match self {
            Self::DirEntry(entr) => entr.length_of_archive(),
            Self::FileEntry{name: _,checksum: _, file_length} => *file_length
        }
    }
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
    pub fn length_of_archive(&self) -> u64 {
        self.length + self.entries.iter().fold(0, |a, e| {
            a + e.length()
        })

    }

    pub fn read_from<T: Read>(reader: &mut T) -> Result<Self, Box<dyn Error>> {
        let mut uintbuf: [u8; 8] = [0; 8];
        let _ = reader.read(&mut uintbuf)?;

        let name_len = u64::from_le_bytes(uintbuf);
        let mut name_bytes = vec![0; name_len as usize];
        let _ = reader.read(&mut name_bytes)?;
        let name = String::from_utf8(name_bytes)?;

        let mut entries = vec![];
        let _ = reader.read(&mut uintbuf)?;
        let entries_len = u64::from_le_bytes(uintbuf);
        for _ in 0..entries_len {
            let _ = reader.read(&mut uintbuf)?;
            let mut boolb = [0; 1];
            let _ = reader.read(&mut boolb)?;
            let is_dir =  match boolb[0] {
                0 => false,
                _ => true,
            };

            let _  = reader.read(&mut uintbuf)?;
            let file_length = u64::from_le_bytes(uintbuf);
            let mut csumbuf: [u8; 32] = [0; 32];
            let _ = reader.read(&mut csumbuf);

            let _ = reader.read(&mut uintbuf);
            let name_length = u64::from_le_bytes(uintbuf);
            let mut name_bytes = vec![0; name_length as usize];
            let _ = reader.read(&mut name_bytes);
            let name = String::from_utf8(name_bytes)?;

            match is_dir {
                false => entries.push(ArchiveEntry::FileEntry{name, checksum: csumbuf, file_length}),
                true => entries.push(ArchiveEntry::DirEntry(ArchiveHeader{name, entries: vec![], length: file_length}))
            }
        }
        Ok(Self{name, entries, length: 0})

    }

    pub fn output_to<T: Write>(&mut self, writer: &mut T) -> Result<(), Box<dyn Error>> {
        writer.write(&(self.name.len() as u64).to_le_bytes())?;
        writer.write(self.name.as_bytes())?;
        writer.write(&(self.entries.len() as u64).to_le_bytes())?;

        for e in self.entries.iter() {
            match e.clone() {
                ArchiveEntry::FileEntry {name, checksum, file_length} => {
                    let mut data: Vec<u8> = vec![];
                    data.write(&(false as u8).to_le_bytes())?;
                    data.write(&(file_length as u64).to_le_bytes())?;
                    data.write(&checksum)?;
                    data.write(&(name.len() as u64).to_le_bytes())?;
                    data.write(name.as_bytes())?;

                    writer.write(&((data.len() + 8) as u64).to_le_bytes())?;
                    writer.write(&(*data))?;
                },
                ArchiveEntry::DirEntry(entry) => {
                    let mut data: Vec<u8> = vec![];
                    data.write(&(true as u8).to_le_bytes())?;
                    data.write(&entry.length_of_archive().to_le_bytes())?;
                    for _ in 0..32 {
                        data.write(&(0 as u8).to_le_bytes())?;
                    }
                    data.write(&(entry.name.len() as u64).to_le_bytes())?;
                    data.write(entry.name.as_bytes())?;

                    writer.write(&((data.len() + 8) as u64).to_le_bytes())?;
                    writer.write(&(*data))?;
                },
            }
        }

        Ok(())
    }

}
