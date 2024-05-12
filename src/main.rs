use std::process::{Command, Stdio};

struct SSHConnect;

impl SSHConnect {
    fn connect(username: &str, hostname: &str, port: &str) {
        let ssh_command = format!("ssh -p {} {}@{}", port, username, hostname);

        match Command::new("sh")
            .arg("-c")
            .arg(&ssh_command)
            .stdin(Stdio::inherit())
            .stdout(Stdio::inherit())
            .stderr(Stdio::inherit())
            .status()
        {
            Ok(status) => {
                if !status.success() {
                    eprintln!("Failed to execute SSH command: {}", status);
                }
            }
            Err(err) => {
                eprintln!("Failed to execute SSH command: {}", err);
            }
        }
    }
}

fn main() {
    let username = "";
    let hostname = "";
    let port: &str = "";

    SSHConnect::connect(username, hostname, port);
    println!("Connection closed!")
}