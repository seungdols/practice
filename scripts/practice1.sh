#!/usr/local/bin/bash

osascript << EOT
tell application "iTerm"
    activate
    tell first window
        create tab with default profile    
    end tell
end tell
EOT

