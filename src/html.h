const char MAIN_PAGE[] = R"=====(
<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"
        integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">

    <style>
    hr { 
    display: block;
    margin-end: auto;
    margin-start: auto;
    border: 1px inset;
    overflow: hidden;
    margin-before: 0.5em;
    margin-after: 0.5em;
}
        .btn {
            width: 100%;
            margin: 5px;
        }

        .my-text {
            font-size: 20px;
            padding: 10px;
        }

        .my-div {
            margin-bottom: 15px;

        }
        .div-my {
  align-items: center
}
    </style>

    <title>CCKIT Main</title>
</head>

<body>
    <div class="container">
     
        <div class="row">
            <div class="col-md-12 col-xs-12">
             <iframe id="iframe1" width="100%" height="300"style="border:none;" src="http://192.168.1.150/" >
  <p>Your browser does not support iframes.</p>
</iframe>
                <h1 class="text-center">CCKIT Main</h1>
            </div>
        </div>

        <hr>

        <div class="row">

           

            <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">Controller</h2>
                </div>
                <hr>
                <div class="row">
                  <div class="col-md-8 col-xs-8 div-my">
                 <p style="text-align:center">Target Temperature</p>
                </div>
                    <div class="col-md-8 col-xs-8 my-div">
                     
                        <input id="r" type="range" min="0" max="60" value="0" onchange="updateSliders()">
                    </div>
                    <div class="col-md-4 col-xs-4 my-div text-center">
                        <span id="rVal">0%</span>
                    </div>
                </div>
                <div class="row">
               <div class="col-md-8 col-xs-8 div-my">
                <p style="text-align:center">Target Humidity</p>
                </div>
                    <div class="col-md-8 col-xs-8 my-div">
                        <input id="g" type="range" min="0" max="100" value="0"onchange="updateSliders()">
                        
                    </div>
                    <div class="col-md-4 col-xs-4 my-div text-center">
                        <span id="gVal">0%</span>
                    </div>
                </div>

                    
                    <button id="prcButton" onclick="sendRGB()" type="button" class="btn btn-primary">Process</button>
                    <button onclick="wifiConfigMode()" type="button" class="btn btn-primary">Start Wifi Config</button>

                </div>

               
             <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">Monitor</h2>
                </div>
                <hr>
                <div >
                <p style="text-align:center;font-size:160%;">Current Temperature</p>
                </div>
                <div class="col-md-12 col-sm-12 text-center my-text">
                    ADC Value: <span id="tempVal">0C</span>
                </div>
                <div class="col-md-12 col-sm-8 text-center my-text">
                <p style="text-align:center;font-size:160%;">Current Humidity</p>
                </div>
                <div class="col-md-12 col-sm-12 text-center my-text">
                    ADC Value: <span id="humVal">0C</span>
                </div>
            </div>
        </div>
    </div>

    <footer class="container footer">
        <div class="col-md-12 text-center">

        </div>
    </footer>

    <script>
   function setIframeSrc() {
  var s = "http://192.168.1.150/";
  var iframe1 = document.getElementById('iframe1');
  iframe1.src = s;
  setTimeout(function(){
      if (window.stop) {
          window.stop();
      } else {
          document.execCommand('Stop'); // MSIE
      }
  }, 5000);
}
setTimeout(setIframeSrc, 5000);

function updateSliders()
{
     var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;
            document.getElementById('rVal').innerHTML = String(redVal)+"C";
            document.getElementById('gVal').innerHTML = String(greenVal)+"%";
}
        function sendRGB() {
            //get rgb values from sliders
            var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;
            //var ssidVal = document.getElementById('s').value;
           // var checkboxVal = document.getElementById('checkbx').checked;

            // convert rgb values range from 0 - 100 to 0 - 255
            var r = parseInt(redVal ).toString();
            var g = parseInt(greenVal ).toString();

            // show slider value in labels
            document.getElementById('rVal').innerHTML = String(redVal)+"C";
            document.getElementById('gVal').innerHTML = String(greenVal)+"%";
           // document.getElementById('txt').innerHTML = String(ssidVal);

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };

            // create querrystring
            var sendStr = "setRGB?r=" + r + "&g=" + g ;
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }

        setInterval(function () {
            // Call a function repetatively with 2 Second interval
            getData();
        }, 1000); //2000mSeconds update rate

        function getData() {
            
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("tempVal").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "readADC", true);
            xhttp.send();

             var xhttp2 = new XMLHttpRequest();
            xhttp2.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("prcButton").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp2.open("GET", "readPrcButton", true);
            xhttp2.send();
        }
          function wifiConfigMode() {

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };

            // create querrystring
            var sendStr = "setWifiMode?w=1";
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }
    </script>

    <script src="https://code.jquery.com/jquery-3.4.1.slim.min.js"
        integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n"
        crossorigin="anonymous"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"
        integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa"
        crossorigin="anonymous"></script>
</body>

</html>
)=====";
const char PAGE_404[] = R"====(
<html>
    <head>
        <title>404</title>
    </head>
    <body>
        <center><h1>ERROR CODE 404</h1></center>
        <center><h1>REQUESTED PAGE NOT FOUND</h1></center>
    </body>
</html>
)====";














const char SETTINGS_PAGE[] = R"=====(
<!doctype html>
<html lang="en">
<head>  
<meta name="viewport" content="width=device-width, initial-scale=1">  
<title> Login Page </title>  
<style>   
Body {  
  font-family: Calibri, Helvetica, sans-serif;  
  background-color: #099bdd;  
}  
.vertical-center {
  margin: 0;
  position: absolute;
  top: 50%;
  -ms-transform: translateY(-50%);
  transform: translateY(-50%);
}
button {   
       background-color: #185abc;   
       width: 50%;  
        color: white;   
        padding: 15px;   
          margin-top: 20px;
    margin-bottom: 20px;
    margin-right:auto;
    margin-left:auto;
        border: 1px solid black;   
        cursor: pointer;  
        border-radius: 6px; 
         }   
 form {   
        //border: 3px solid #f1f1f1;   

    }   
 input[type=text], input[type=password] {   
        width: 100%;   
        margin: 8px 0;  
        padding: 12px 20px;   
        display: inline-block;   
        border: 2px solid black;   
        box-sizing: border-box;   
    }  
 button:hover {   
        opacity: 0.7;   
    }   
  .cancelbtn {   
        width: auto;   
        padding: 10px 18px;  
        margin: 10px 5px;  
    }   
        
     
 .container {  
     margin: auto;
      width: 80%; 
        padding: 25px;   
        background-color: white;
         border-radius: 15px; 
         text-align: center;
    }   
    .mydiv {
    margin-right:auto;
    margin-left:auto;
     display: flex;
  flex-direction: row;
  justify-content: center;
}
</style>   
</head>    
<body>    
   
    <form>  
        <div class="container">   
         <center> <h1> CCKIT Wifi Settings </h1> </center>   
         <hr >
         <div class="mydiv">
          <b clabel id="camStatus"></b>
          </div>
           <hr  >
            <label>SSID</label>   
            <input type="text" placeholder="Enter SSID" id="ssidText" required>  
            <label>Password</label>   
            <input type="password" placeholder="Enter Password" id="passwordText" required>  
     <button type="submit"  onclick="sendRGB()">Process</button>   
           <div class="mydiv">
          <h4 clabel id="msg"></h4>
          </div>
        </div>   
    </form>     
</body>     

    <script>

        function sendRGB() {
            //get rgb values from sliders
            var passVal = document.getElementById('passwordText').value;
            var ssidVal = document.getElementById('ssidText').value;

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };

            // create querrystring
            var sendStr = "setWifi?s=" + ssidVal + "&p=" + passVal;
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }

           setInterval(function () {
            // Call a function repetatively with 2 Second interval
            notifyCam();
            getMessage();
        }, 1000); //2000mSeconds update rate

        function notifyCam() {
            
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("camStatus").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "notifyCAM", true);
            xhttp.send();
        }
         function getMessage() {
            
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("msg").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "message", true);
            xhttp.send();
        }
    </script>

    <script src="https://code.jquery.com/jquery-3.4.1.slim.min.js"
        integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n"
        crossorigin="anonymous"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"
        integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa"
        crossorigin="anonymous"></script>
</body>

</html>
)=====";



