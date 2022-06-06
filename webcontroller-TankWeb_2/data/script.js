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
  

  var push1 = document.getElementsByClassName("push1");
  var carP = document.getElementById("carPos");
  carP.innerHTML = push1[4].value; //顯示預設的值 Stop
  car = push1[4].name;
  for (var i = 0; i < push1.length; i++) {
    push1[i].onclick = function () {
      carP.innerHTML = this.value;
      car = this.name;
      value("car", car);
    }
  }
  var push2 = document.getElementsByClassName("push2");
  var motoP = document.getElementById("motoPos");
  motoP.innerHTML = push2[1].value; //顯示預設的值 一
  moto = push2[1].value;
  for (var i = 0; i < push2.length; i++) {
    push2[i].onclick = function () {
      motoP.innerHTML = this.value;
      moto = this.name;
      value("moto",moto);
    }
  }
  var push3 = document.getElementsByClassName("push3");
  var YuntaiP = document.getElementById("YuntaiPos");
  YuntaiP.innerHTML = push3[2].value; //顯示預設的值 Stop
  Yuntai = push3[2].name;
  for (var i = 0; i < push3.length; i++) {
    push3[i].onclick = function () {
      YuntaiP.innerHTML = this.value;
      Yuntai = this.name;
      value("Yuntai",Yuntai);
    }
  }
  var servo = document.getElementsByClassName("servo");
  var servoP = document.getElementById("servoPos");
  servoP.innerHTML = servo[2].value; //顯示預設的值 Stop
  Servo = servo[2].name;
  for (var i = 0; i < servo.length; i++) {
    servo[i].onclick = function () {
      servoP.innerHTML = this.value;
      Servo = this.name;
      value("value",Servo);
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