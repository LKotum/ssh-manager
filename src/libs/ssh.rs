use std::process::{Command, Stdio};
pub struct SSHConnect;

impl SSHConnect {
    pub fn connect(username: &str, hostname: &str, port: &str) {
        match Command::new("ssh")
            .arg("-p")
            .arg(port)
            .arg(format!("{}@{}", username, hostname))
            .stdin(Stdio::inherit())
            .stdout(Stdio::inherit())
            .stderr(Stdio::inherit())
            .status()
        {
            Ok(status) => {
                if !status.success() {
                    eprintln!("Failed to execute SSH command: {}", status);
                }
                else {
                    println!("Connection closed!")
                }
            }
            Err(err) => {
                eprintln!("Failed to execute SSH command: {}", err);
            }
        }
    }
}