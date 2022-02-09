use structopt::{StructOpt, self};

use sha2::Sha256;
use sha2::Digest;
use std::fs::File;
use std::io::copy;

use std::error::Error;

#[derive(Debug)]
enum ArchiveEntry {
    FileEntry {name: String, checksum: [u8;32], file_length: u64},
    DirEntry(ArchiveHeader),
}

#[derive(Debug)]
enum ArchiveError {
    TempError
}

impl std::fmt::Display for ArchiveError {
    fn fmt(&self, fmt: &mut std::fmt::Formatter)  -> Result<(), std::fmt::Error> {
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
        Err(Box::new(ArchiveError::TempError))
    }

    pub fn new(name: &str) -> Result<Self, Box<dyn Error>> {
        if std::fs::metadata(name)?.is_dir() {
            Self::new_dir(name)
        } else {
            Self::new_file(name)
        }
    }
}

#[derive(Debug)]
pub struct ArchiveHeader {
    name: String,
    entries: Vec<Box<ArchiveEntry>>
}

#[derive(Debug, StructOpt)]
pub struct Args {
    #[structopt(short="c", long="compress")]
    pub compress: bool,

    #[structopt(short="f", long="output-file")]
    pub output_file: Option<String>,

    pub files: Vec<String>,
}

fn main() {
    let args = Args::from_args();

    for fname in args.files {
        println!("{:?}", ArchiveEntry::new(&fname));
    }
}
