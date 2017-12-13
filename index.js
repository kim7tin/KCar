var ws, connected = false, roboState = false, light = false;
function start(websocketServerLocation) {
	ws = new WebSocket(websocketServerLocation);
	ws.onopen = function() {
		connected = true;
		console.log("Connected");
	};
	ws.onmessage = function(evt) {
	};
	ws.onclose = function() {
		connected = false;
		console.log("Connection is closed");
		setTimeout(function() {
			start(websocketServerLocation);
		}, 10);
	};
}
start('ws://' + location.hostname + ':81/', [ 'arduino' ]);

window.oncontextmenu = function(event) {
	event.preventDefault();
	event.stopPropagation();
	return false;
};

document.body.addEventListener('touchmove', function(event) {
	event.preventDefault();
}, false);

document.getElementById("UpStart").addEventListener("touchstart",
		function(evt) {
			UpStart(1);
		}, false);
document.getElementById("UpStart").addEventListener("touchend", function(evt) {
	UpStart(0);
}, false);
document.getElementById("DownStart").addEventListener("touchstart",
		function(evt) {
			DownStart(1);
		}, false);
document.getElementById("DownStart").addEventListener("touchend",
		function(evt) {
			DownStart(0);
		}, false);
document.getElementById("LeftStart").addEventListener("touchstart",
		function(evt) {
			LeftStart(1);
		}, false);
document.getElementById("LeftStart").addEventListener("touchend",
		function(evt) {
			LeftStart(0);
		}, false);
document.getElementById("RightStart").addEventListener("touchstart",
		function(evt) {
			RightStart(1);
		}, false);
document.getElementById("RightStart").addEventListener("touchend",
		function(evt) {
			RightStart(0);
		}, false);

document.getElementById("TransformStart").addEventListener("click",
		function(evt) {
			if (connected) {
				var state = roboState ? "D" : "U";
				ws.send("ROBUP" + state);
				roboState = ~roboState;
			}
		}, false);

document.getElementById("LightStart").addEventListener("click",
		function(evt) {
			if (connected) {
				var state = light ? "0" : "1";
				ws.send("LIGHT" + state);
				light = ~light;
			}
		}, false);

function UpStart(state) {
	if (connected) {
		ws.send("UPSTA" + state);
	}
}

function DownStart(state) {
	if (connected) {
		ws.send("DOSTA" + state);
	}
}

function LeftStart(state) {
	if (connected) {
		ws.send("LESTA" + state);
	}
}

function RightStart(state) {
	if (connected) {
		ws.send("RISTA" + state);
	}
}