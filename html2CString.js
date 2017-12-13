function convert() {
	var lines = document.getElementById("input").value.split('\n');
	console.log(lines);
	var output = "String page=\"\";\n";
	for (var i = 0; i < lines.length; i++) {
		output += "page+=\"";
		output += lines[i].replace(/\"/g, '\\\"');
		output += "\";\n";
	}
	document.getElementById("output").value = output;
}