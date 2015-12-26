## Texada ##

# Installing Client-side Dependencies #

# Pre-requisites #
1. node.js - Download and install using the following Terminal commands
  1. sudo apt-get purge nodejs npm
  2. sudo apt-get install -y python-software-properties python g++ make software-properties-common
  3. sudo add-apt-repository ppa:chris-lea/node.js
  4. sudo apt-get update
  5. sudo apt-get install nodejs
2. Go - Download and install using the following Terminal commands
  1. sudo apt-get install -y python-software-properties python g++ make software-properties-common
  2. sudo add-apt-repository ppa:gophers/go
  3. sudo apt-get update
  4. sudo apt-get install golang


# Installation (Unix-based systems) #
1. Open Terminal
2. cd {TEXADA_HOME}/www/client
3. sudo npm install
4. cd app
5. ../node_modules/bower/bin/bower install


# Running the server (Unix-based systems) #
1. Open Terminal and enter the following commands
2. cd {TEXADA_HOME}/www
3. go run server.go {TEXADA_HOME}/texada {PORT}

URL for the front end - http://localhost:{PORT}/texada