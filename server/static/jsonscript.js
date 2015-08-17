 $( document ).ready(function() {
         var out_obj = {}
         var output_loaded = false
         var all_out = '<td id = "allout" valign="top"> <h3> Output: </h3> <div id="result" class="output" style="display:block"> <table> <tr><td><b> Bindings <b></td><td> <div class="onoffswitch"> <input type="checkbox" name="onoffswitch" class="onoffswitch-checkbox" id="myonoffswitch"> <label class="onoffswitch-label" for="myonoffswitch"> <span class="onoffswitch-inner"></span> <span class="onoffswitch-switch"></span> </label> </div></td><td> <b> Property Instantiations </b></td></tr> </table><table> <tr id="realout"> <td> </td></tr></table> </div> </td>'
         var bindings_str = ""
         var instants_str = ""
         $("#form1").submit(function(event) {
             event.preventDefault() // don't send data with standard form tactic
             // TODO: apparenty .val() on text area does weird things with carriage return:
             // http://api.jquery.com/val/
             var in_str = "{\"log\" : \"" + $("#log").val().replace(/\n/g, '\\n') + "\", \"args\" : \"" + $("#args").val() + "\"}"
            $.post("/texada/mine/", in_str, function( data ) {
              out_obj = jQuery.parseJSON(data)
              var instants = out_obj["prop-instances"]
              bindings_str = "";
              instants_str = "";
              for (var i = 0; i < instants.length; i++){ 
                  var l = i + 1
                  bindings_str += l + ". ["
                  instants_str += l + ". "
                  for (var prop in instants[i]){
                     bindings_str += prop + ": " + instants[i][prop] + ", ";
                  }
                  // remove last comma
                  bindings_str = bindings_str.slice(0,-2)
                  bindings_str +=  "]<br>"
                 instants_str += instantiateString(out_obj["prop-type"]["vars"], instants[i], out_obj["prop-type"]["str"])+ "<br>"
              }

              if (instants.length == 0){
                  output_loaded = false
                  $("#allout").replaceWith('<td id = "allout" valign="top"><h3>Output:</h3><div id="result" class="output" style="display:block"> No valid instantiations. </div> </td>')
              } else{

              // decide which format to output
              if ($("#myonoffswitch").is(":checked")){
                if (!output_loaded)
                   $("#allout").replaceWith(all_out)
                $("#realout").replaceWith('<tr id="realout"> <td>' + instants_str + '</td></tr>')
                output_loaded = true
              } else {
                 if (!output_loaded)
                    $("#allout").replaceWith(all_out)
                 $("#realout").replaceWith('<tr id="realout"> <td>' + bindings_str + '</td></tr>')
                 output_loaded = true
              }}
            });
         });

        // switch output format when switch button is clicked
         $('body').on('click','#myonoffswitch', function() {
              if ($("#myonoffswitch").is(":checked")){
                $("#realout").replaceWith('<tr id="realout"> <td>' + instants_str + '</td></tr>')
              } else {
                 $("#realout").replaceWith('<tr id="realout"> <td>' + bindings_str + '</td></tr>')
              }
         });

    });

// helper to instantiate strings
function instantiateString(var_posns, var_binds, str) {
    var out_str = str
    var instant_helper = []
    for (var key in var_posns) {
        var length = key.length
        var binding = var_binds[key]
        for (var k = 0; k <var_posns[key].length ; k++ ) {
            instant_helper.push({"new" : binding, "start" : var_posns[key][k], "end" :  var_posns[key][k] + length})
        } 
    }
    instant_helper.sort(function(a,b) {return b["start"]- a["start"]}) 

   for (var j = 0; j < instant_helper.length; j++) {
       str = str.slice(0, instant_helper[j]["start"]) + instant_helper[j]["new"] + str.slice(instant_helper[j]["end"])
   }
   return str
   
}








