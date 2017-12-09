#!/usr/local/bin/bash

#
# Goal: 4분할 화면 
# TODO: 
# 1. create tab 
# 2. create session       
# 3.    split horizontally
# 4. create split vertically each session

osascript << EOT

tell application "iTerm"

    tell current window 
        set dev_tab to create tab with default profile
        select dev_tab
    end tell

    tell current session of current window
        write text "echo 'pane1'" 
        set pane2 to split vertically with default profile
        tell pane2 
            write text "echo 'pane2'"
            set pane4 to split horizontally with default profile 
            tell pane4 
                write text "echo 'pane4'"
            end tell
        end tell
        set pane3 to split horizontally with default profile
        tell pane3 
            write text "echo 'pane3'"
        end tell 
    end



end tell


EOT
