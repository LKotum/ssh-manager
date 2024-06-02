use std::io::{self, stdout};

use ratatui::{
    prelude::*,
    widgets::*,
};

use crossterm::{
    event::{self, Event, KeyCode},
    terminal::{
        disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen,
    },
    ExecutableCommand,
};
