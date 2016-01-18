# Texada web-server installation instructions #

## Pre-requisites ##

The Texada web-server depends on (1) node.js and (2) Go lang. Here are the steps for installing these dependencies on Ubuntu (tested with Ubuntu 12.04.5).

Install node.js:

Node.JS is needed to install further dependencies for the web application like Bower, Protractor, Jasmine, etc. It can be installed using apt-get on Linux systems. A detailed step-by-step guide is given below:

  1. sudo apt-get purge nodejs npm
  2. sudo apt-get install -y python-software-properties python g++ make software-properties-common
  3. sudo add-apt-repository ppa:chris-lea/node.js
  4. sudo apt-get update
  5. sudo apt-get install nodejs

Install Go lang:

Go lang is needed to run the web server that hosts the web version of Texada. It can be installed using apt-get on Linux systems. A detailed step-by-step guide is given below:

  1. sudo apt-get install -y python-software-properties python g++ make software-properties-common
  2. sudo add-apt-repository ppa:gophers/go
  3. sudo apt-get update
  4. sudo apt-get install golang


# Install the node.js web-app #

1. cd {TEXADA_HOME}/www/client
2. sudo npm install
3. cd app
4. ../node_modules/bower/bin/bower install

# Deploying updates #

1. Deploy updated files (or updated version of the codebase)
2. cd {TEXADA_HOME}/www/client
3. sudo npm install
4. cd app
5. ../node_modules/bower/bin/bower install



# Running the server #

1. cd {TEXADA_HOME}/www
2. go run server.go {TEXADA_HOME}/texada {PORT}

Once the server is up and running, it can be accessed using a web browser.
The default front-end URL is: http://localhost:{PORT}/texada