/*setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var obj = JSON.parse(this.responseText);
            carP.innerHTML = direction[obj.dir - 1].value;
            powP.innerHTML = obj.pwm;
            mode.checked = obj.mode;
            if (obj.mode == 1 ) {
                modeP.innerHTML = "Auto";
                mode.checked = true
            }
            else {
                modeP.innerHTML = "Manual";
                mode.checked = false;
            }

        }
    };
    xhttp.open("GET", "/state", true);
    xhttp.send();
},2000);*/

if (!!window.EventSource) {
    var source = new EventSource('/events');
  
    source.addEventListener('open', function(e) {
      console.log("Events Connected");
    }, false);
  
    source.addEventListener('error', function(e) {
      if (e.target.readyState != EventSource.OPEN) {
        console.log("Events Disconnected");
      }
    }, false);
  
    source.addEventListener('data_readings', function(e) {
      console.log("data_readings", e.data);
      var obj = JSON.parse(e.data);
        carP.innerHTML = direction[obj.dir - 1].value;
        powP.innerHTML = obj.pwm;
        mode.checked = obj.mode;
        swP.innerHTML = obj.sw;
        if (obj.mode == 1) {
            modeP.innerHTML = "Auto";
            mode.checked = true
        }
        else {
            modeP.innerHTML = "Manual";
            mode.checked = false;
        }
    }, false);
  
  }
  

var direction = document.getElementsByClassName("direction");
    var carP = document.getElementById("carPos");
    carP.innerHTML = direction[4].value; //顯示預設的值 Stop
    car = direction[4].name;
    for (var i = 0; i < direction.length; i++) {
      direction[i].onclick = function () {
        carP.innerHTML = this.value;
        car = this.name;
        value("dir",car);
      }
    }
    var mode = document.getElementById("mode");
    var modeP = document.getElementById("modePos");
    modeP.innerHTML = "Manual";
    M = mode.checked;
    mode.onchange = function () {//接收
      if (mode.checked)
      {
        modeP.innerHTML = "Auto";
        M = 1;
      }
      else
      {
        modeP.innerHTML = "Manual";
        M = 0;
      }
      
      value("mode",M);
    }

    var Value = document.getElementById("powvalue");
    var powP = document.getElementById("powPos");
    powP.innerHTML = Value.value; //顯示預設的值 會顯示中間值
    Value.oninput = function () {
      Value.value = this.value;
      powP.innerHTML = this.value;
      delayShowData("pwm",Value.value);
    }
    var SwValue = document.getElementById("swvalue");
    var swP = document.getElementById("swPos");
    swP.innerHTML = Value.value; //顯示預設的值 會顯示中間值
    SwValue.oninput = function () {
      SwValue.value = this.value;
      swP.innerHTML = this.value;
      delayShowData("sw",SwValue.value);
    }
    var timers = {};
      function delayShowData(type, values) {
        clearTimeout(timers[type]);
        timers[type] = setTimeout(function () {
          $('span.' + type).text(values[0] + 'mm - ' + values[1] + 'mm');
          value(type, values);
        }, 300);
      }
    $.ajaxSetup({ timeout: 1000 });
    function value(type,pos) {
      $.get("/?"+ type +"=" + pos + "&");
      { Connection: close };
}