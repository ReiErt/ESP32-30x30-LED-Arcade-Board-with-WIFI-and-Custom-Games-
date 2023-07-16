//lobby logic functions

function createLobby(){
    var gameID = localStorage.getItem("GameID");
    parseInt(gameID);//parsen der Stringwerte im localstorage zu int für die array werte
    var max_players = localStorage.getItem("MaxPlayer");
    parseInt(max_players);
    var map = 0; 
    if(gameID == 2)//Abfrage für Othello
        map = localStorage.getItem("Map");
    socket = new WebSocket("ws://192.168.4.1/ws");
    socket.onopen = function() {
        array = new Uint8Array(4);
        //Kennzeichner für create Lobby
        array[0] = 1;
        //Gamme ID retrieved from localstorage, set in gameselect window
        array[1] = gameID;
        //MaxPlayer retrieved from localstorage, set in gameselect window
        array[2] = max_players;
        array[3] = map;
        socket.send(array);
    }
}

function reqListener() {
    var a = new Int8Array(xhttp.response);
    if(a[0]==1||a[0]==2){    // wenn lobby offen/running -> umschalten der sichtbarkeiten
        document.getElementById("startnew").classList.add("hidden");
        document.getElementById("myPopup").classList.remove("hidden");
    }
    else 
    document.getElementById("myPopup").classList.add("hidden");
}

function getData() {        //HttpRequest an Backend für Abfragen
    console.log("I got called");
    xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/Data");
    xhttp.responseType = "arraybuffer";
    xhttp.send();
    xhttp.addEventListener("load", reqListener);
}

function joinLobby(){
    socket = new WebSocket("ws://192.168.4.1/ws");
    socket.onopen = function() {
        array = new Uint8Array(1);
        //Kennzeichner für Aktion Join
        array[0] = 2;
        socket.send(array);
    }
}

function Button(a){
    array = new Uint8Array(3);
    //Kennzeichner für Knopf
    array[0] = 0;
    //KnopfID
    array[1] = a[0];  
    //Gedrückt(1), losgelassen(0)
    array[2] = a[1];  
    socket.send(array);
}
//mögliche funktion für debugging der Spieleranzahl, benötigt aufruf falls gewünscht
function getPlayers() {
    var socket = new WebSocket("ws://192.168.4.1/ws");
    socket.onopen = function() {
        array = new Uint8Array(1);
        array[0] = 3;
        socket.send(array);
    }
}

