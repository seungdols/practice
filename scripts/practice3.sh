#!/usr/local/bin/bash 

osascript << EOT
tell application "iTerm"

    # Create tab.
    #tell current window
    #    create tab with default profile
    #end tell

    # Create session's 
    tell current session of current window
        set test to (split horizontally with default profile)
    end tell

    #tell test 
    #    write text "echo 'test'"
    #end tell

    #tell session 1 of current tab of current window
    tell session 1 of current tab of current window
        write text "echo 'session1'"
        set test1 to (split vertically with default profile command "tmux split-pane")
    end tell


end tell
EOT
