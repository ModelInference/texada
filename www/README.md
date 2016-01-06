# Texada web-server installation instructions #

## Pre-requisites ##

The Texada web-server depends on (1) node.js and (2) Go lang. Here are the steps for installing these dependencies on Ubuntu (tested with Ubuntu 12.04.5).

Install node.js:

Add a comment to this line

  1. sudo apt-get purge nodejs npm
  2. sudo apt-get install -y python-software-properties python g++ make software-properties-common
  3. sudo add-apt-repository ppa:chris-lea/node.js
  4. sudo apt-get update
  5. sudo apt-get install nodejs

Install Go lang:

Add a comment to this line

  1. sudo apt-get install -y python-software-properties python g++ make software-properties-common
  2. sudo add-apt-repository ppa:gophers/go
  3. sudo apt-get update
  4. sudo apt-get install golang


# Install the node.js web-app #

1. cd {TEXADA_HOME}/www/client
2. sudo npm install
3. cd app
4. ../node_modules/bower/bin/bower install


# Running the server #

1. cd {TEXADA_HOME}/www
2. go run server.go {TEXADA_HOME}/texada {PORT}

Add a comment to this line
The default front-end URL is: http://localhost:{PORT}/texada