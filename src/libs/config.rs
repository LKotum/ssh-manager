use std::path::Path;
use std::{fs, io};
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize)]
struct SSHConfig {
    host: String,
    user: String,
	port: String
}

impl Default for SSHConfig {
    fn default() -> Self {
        Self {
            host: "localhost".to_string(),
			user: "root".to_string(),
			port: "22".to_string()
        }
    }
}

enum ConfigError {
    IoError(io::Error),
    InvalidConfig(toml::de::Error)
}

impl From<io::Error> for ConfigError {
    fn from(value: io::Error) -> Self {
        Self::IoError(value)
    }
}

impl From<toml::de::Error> for ConfigError {
    fn from(value: toml::de::Error) -> Self {
        Self::InvalidConfig(value)
    }
}

fn load_or_initialize() -> Result<SSHConfig, ConfigError> {
    let config_path = Path::new("Config.toml");

    if config_path.exists() {
        let content = fs::read_to_string(config_path)?;
        let config = toml::from_str(&content)?;

        return Ok(config);
    }

    let config = SSHConfig::default();
    let toml = toml::to_string(&config).unwrap();

    fs::write(config_path, toml)?;
    Ok(config)
}