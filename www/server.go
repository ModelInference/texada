/* This is an Go implementation of a simple web-server custom-made for
the Texada website. Works with go version 1.2.1.

The server communicates with the front-end using JSON messages,
handles file uploads, and invokes the Texada binary to do the real
work of mining property instances.

Usage: go run server.go [texada-bin] [port]

- texada-bin: location of the texada binary
- port: TCP port on which the server will listen to HTTP GET requests
*/

package main

import (
	"encoding/json"
	"fmt"
	"html"
	"io"
	"io/ioutil"
	"math/rand"
	"net/http"
	"os"
	"os/exec"
	"strings"
	"time"
)

// Texada output, which is the input to the webpage.
type Output struct {
	OutputTitle   string
	OutputJSON    string
	OutputDisplay string
}

// Texada input.
type Input struct {
	Log  string `json:"log"`
	Args string `json:"args"`
}

// Where to find the texada binary.
var texadaCmd string

// Outputs an error string to stdout.
func printErrorStr(s string) {
	fmt.Println(time.Now(), "Error: "+s)
}

// Outputs the received http request to stdout.
func printRequest(r *http.Request) {
	fmt.Println(time.Now(), "REQ:", html.EscapeString(r.URL.Path), ", ip/port: ", r.RemoteAddr)
}

// Does the mining.
func mine(log string, args string, w http.ResponseWriter) {
	// Remove carriage returns from the input, and convert to []byte.
	log = strings.Replace(log, "\r", "", -1)
	args = strings.Replace(args, "\r", "", -1)
	logbytes := []byte(log)
	argsbytes := []byte(args)

	// Save log to file.
	logfile, err := ioutil.TempFile("/tmp/", "log-")
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	if ioutil.WriteFile(logfile.Name(), logbytes, 0600) != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// Save args to file.
	argsfile, err := ioutil.TempFile("/tmp/", "args-")
	if err != nil {
		os.Remove(logfile.Name())
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	if ioutil.WriteFile(argsfile.Name(), argsbytes, 0600) != nil {
		os.Remove(logfile.Name())
		os.Remove(argsfile.Name())
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	outbytes, err := exec.Command(texadaCmd, "--output-json", "-c",
		argsfile.Name(), logfile.Name()).Output()

	if err != nil {
		fmt.Println(err.Error())
		os.Remove(logfile.Name())
		os.Remove(argsfile.Name())
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	os.Remove(logfile.Name())
	os.Remove(argsfile.Name())
	jsonstr := strings.TrimSpace(string(outbytes))
	w.Write([]byte(jsonstr))
	// fmt.Println(jsonstr)
	// cmdfull := cmd + " -c " + argsfile.Name() + " " + logfile.Name()
}

// Handles POST to /texada/mine/
func mineHandler(w http.ResponseWriter, r *http.Request) {
	printRequest(r)

	// Retrieve  data.
	decoder := json.NewDecoder(r.Body)
	var in Input
	err := decoder.Decode(&in)
	if err != nil {
		fmt.Println(err.Error())
	}
	log := in.Log
	args := in.Args
	mine(log, args, w)

}

// Handle file upload and mine the uploaded file with given args
func uploadMineHandler(w http.ResponseWriter, r *http.Request) {

	// The args to be mined
	args := r.FormValue("args")

	// The file to be mined
	file, _, err := r.FormFile("file")

	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	defer file.Close()

	// Create random string to be used as temporary file name (for file upload)
	randString := RandStringBytes(20)

	// Create file with the generated random string
	out, err := os.Create(randString)

	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// will close the connection later
	defer out.Close()

	// write the content of the uploaded file to the temporary file
	_, err = io.Copy(out, file)

	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// Read contents of uploaded file
	fileContent, err := ioutil.ReadFile(randString)

	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// Remove temporary file
	err = os.Remove(randString)

	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// stringify contents of uploaded file
	logContent := string(fileContent)

	printRequest(r)

	// mine the uploaded file with the given args
	mine(logContent, args, w)

}

// Create random string of letters
func RandStringBytes(n int) string {
	// Letters to be used for creating random file name
	const letterBytes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	b := make([]byte, n)
	for i := range b {
		b[i] = letterBytes[rand.Intn(len(letterBytes))]
	}
	return string(b)
}

func main() {
	// Handle command line args:
	args := os.Args
	if len(args) != 3 {
		printErrorStr("Wrong number of arguments.")
		fmt.Println("Usage: go run server.go [texada-bin] [port]")
		fmt.Println("Examples:")
		fmt.Println("\tgo run server.go /Users/ivan/repos/texada/texada 8080")
		fmt.Println("\tgo run server.go /home/bestchai/texada/texada 8080")
		os.Exit(-1)
	}

	texadaCmd = args[1]
	port := args[2]

	fmt.Printf("Using texada binary: %s\n", texadaCmd)
	fmt.Printf("Using port: %s\n", port)

	// Register handlers for different paths.
	http.HandleFunc("/texada/mine/", mineHandler)
	http.HandleFunc("/texada/uploadMine/", uploadMineHandler)

	// Register static files handler
	http.Handle("/texada/", http.StripPrefix("/texada/", http.FileServer(http.Dir("client/app/"))))
	// Listen on port with default IP.
	http.ListenAndServe(":"+port, nil)
}
