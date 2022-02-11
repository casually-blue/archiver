use clap::Parser;

use std::{
    fs::File,
    error::Error
};

mod archive;
use archive::*;

#[derive(Debug, Parser)]
#[clap(name = "arc", version = "0.1.0", author = "Sierra-Team")]
pub struct Args {

    #[clap(long, short='c')]
    pub compress: bool,

    #[clap(short='f', long="output-file")]
    pub output_file: Option<String>,

    
    #[clap(required=true)]
    pub files: Vec<String>,
}

fn main() -> Result<(), Box<dyn Error>> {
    let args = Args::parse();

    let mut hdr = ArchiveHeader {
        name: args.output_file.unwrap_or(args.files.get(0).unwrap().to_string() + ".arc"),
        entries: vec![],
        length: 0,
    };

    for fname in args.files {
        println!("making entry for: {}", fname);
        let entry = ArchiveEntry::new(&fname)?;
        hdr.entries.push(entry);
        println!("created entry {}", hdr.entries.len() - 1);
    }

    if hdr.entries.len() == 1 {
        match (*hdr.entries.get(0).unwrap()).clone() {
            ArchiveEntry::DirEntry(entry) => {
                let mut entry = entry;
                entry.name = hdr.name;
                hdr = entry;
            }
            _ => {}
        }

    }

    let mut file = File::create(hdr.name.clone())?;

    hdr.output_to(&mut file)?;

    Ok(())
}
