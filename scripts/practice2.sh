#!/usr/local/bin/bash 

# 외부에서 퍼온 건데, 공부하면 될 것 같다. 
osascript << EOT
tell application "iTerm"
    # Create tab.
    tell current window
        create tab with default profile
    end tell

    # Create sessions.
    tell current session of current window
        split horizontally with default profile
        split horizontally with default profile
        split horizontally with default profile
        split horizontally with default profile
    end tell
    tell session 1 of current tab of current window
        set test1 to (split vertically with default profile) 
        tell test1 
            write text "echo test1"
        end tell 
        write text "echo session 1"
    end tell
    tell session 3 of current tab of current window
        split vertically with default profile
        write text "echo session 3"
    end tell
    tell session 5 of current tab of current window
        split vertically with default profile
        write text "echo session 5"
    end tell
    tell session 7 of current tab of current window
        split vertically with default profile
        write text "echo session 7"
    end tell
    tell session 9 of current tab of current window
        split vertically with default profile
        split vertically with default profile
        write text "echo session 9"
    end tell

    # Establish connections & execute actions.
    #repeat with i from 1 to count of commands
    #    tell session i of current tab of current window
    #        write text (connection of item i of commands)
    #        write text (action of item i of commands)
    #    end tell
    #end repeat
end tell
EOT
