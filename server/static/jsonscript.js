 $( document ).ready(function() {
         $("#form1").submit(function(event) {
             event.preventDefault() // don't send data with standard form tactic
             // TODO: apparenty .val() on text area does weird things with carriage return:
             // http://api.jquery.com/val/
             var in_str = "{\"log\" : \"" + $("#log").val().replace(/\n/g, '\\n') + "\", \"args\" : \"" + $("#args").val() + "\"}"
            $.post("/texada/mine/", in_str, function( data ) {
              var out_obj = jQuery.parseJSON(data)
              var prop_type = out_obj["prop-type"]["str"]
              var instants = out_obj["prop-instances"]
              var out_str = "Valid bindings:<br>"
              for (i = 0; i < instants.length; i++){
                  out_str += "["
                  for (var prop in instants[i]){
                     out_str += prop + "->" + instants[i][prop] + ", ";
                  }
                  // remove last comma
                  out_str = out_str.slice(0,-2)
                  out_str +=  "]<br>"
              }
              if (instants.length == 0)
                  out_str += "none"
              $("div.output").replaceWith('<div id="result" class="output" style="display:block">'+ out_str +'</div>');
            });
         });
    });
