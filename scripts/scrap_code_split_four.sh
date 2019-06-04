#1/usr/local/bin/bash 

  osascript << EOT
      tell application "iTerm"
        set win to (create window with default profile)
          tell current session of win
            write text "echo watch:server:dev"
            
            set webpane to (split vertically with default profile)
            tell webpane
              write text "echo watch:web:dev"
              
              set consolepane to (split horizontally with default profile)
              tell consolepane
                write text "echo serve:console"
              end tell
            end tell
            
            set serverpane to (split horizontally with default profile)
            tell serverpane
              write text "echo nodemon"
            end tell
          end tell
        activate
      end tell
EOT
    ;;
esac
