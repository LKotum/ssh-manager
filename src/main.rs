mod libs;
use crate::libs::SSHConnect;

fn main() {
    let username = "maxlane";
    let hostname = "wolfhost.ru";
    let port: &str = "22000";

    SSHConnect::connect(username, hostname, port);
    println!("Connection closed!")
}
