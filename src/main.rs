use structopt::{StructOpt, self};

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

    println!("{:#?}", args);
}
