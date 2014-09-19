package main

import (
	"html/template"
	"io/ioutil"
	"net/http"
	"os/exec"
	"strings"
	"os"
)

// Compile templates on start.
var templates = template.Must(template.ParseFiles("static/index.html"))

// Display the template.
func renderTemplate(w http.ResponseWriter, tmpl string, data interface{}) {
	err := templates.ExecuteTemplate(w, tmpl+".html", data)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

// Handle a mine POST.
func mineHandler(w http.ResponseWriter, r *http.Request) {
	// Retrieve form data.
	body := r.FormValue("body")
	args := r.FormValue("args")
	
	// Remove carriage returns from the input, and convert to []byte
	body = strings.Replace(body,"\r","",-1)
	args = strings.Replace(args,"\r","",-1)
	bodybytes := []byte(body)
	argsbytes := []byte(args)

	// Save body to file.
	logfile, err := ioutil.TempFile("/tmp/", "log-");
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	if ioutil.WriteFile(logfile.Name(), bodybytes, 0600) != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// Save args to file.
	argsfile, err := ioutil.TempFile("/tmp/", "args-");
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
	
	cmd := "/Users/ivan/repos/texada/texada"
	outbytes, err := exec.Command(cmd, "-c", argsfile.Name(), logfile.Name()).Output();
	if err != nil {
		os.Remove(logfile.Name())
		os.Remove(argsfile.Name())
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	
	os.Remove(logfile.Name())
	os.Remove(argsfile.Name())
	outstr := strings.TrimSpace(string(outbytes))
	// cmdfull := cmd + " -c " + argsfile.Name() + " " + logfile.Name()
	renderTemplate(w, "index", "Texada output:\n" + outstr)
}

func viewHandler(w http.ResponseWriter, r *http.Request) {
	renderTemplate(w, "index", nil)
}

func main() {
	http.HandleFunc("/texada/", viewHandler)
	http.HandleFunc("/texada/mine/", mineHandler)
	
	// Static files handler.
	http.Handle("/static/", http.StripPrefix("/static/", http.FileServer(http.Dir("static"))))

	// Listen on port 8080.
	http.ListenAndServe(":8080", nil)
}
