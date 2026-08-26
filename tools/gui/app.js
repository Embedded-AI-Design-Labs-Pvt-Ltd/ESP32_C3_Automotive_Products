(function () {
  var state = null;

  function $(id) {
    return document.getElementById(id);
  }

  function post(url, body) {
    return fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body || {})
    }).then(function (r) { return r.json(); });
  }

  function render(s) {
    state = s;
    $("soc").textContent = s.bms.soc;
    $("pack").textContent = s.bms.pack_v.toFixed(1);
    $("spd").textContent = s.bcm.speed;
    $("trq").textContent = s.vcu.torque;
    $("tmax").textContent = s.bms.tmax;
    $("fault").textContent = s.fault;
    $("fault").className = "fault-" + s.fault;
    $("sess").textContent = s.session;
    $("link").textContent = s.mode === "hardware"
      ? (s.hw_ok ? "ESP32-C3 linked" : "Hardware requested — using Virtual ECU")
      : "Virtual ECU";
    $("btn-virtual").className = s.mode === "virtual" ? "on" : "";
    $("btn-hw").className = s.mode === "hardware" ? "on" : "";
    $("bcm").textContent =
      "ign " + s.bcm.ign + "\ndoors " + s.bcm.doors + "\nlights 0x" + s.bcm.lights.toString(16) + "\nspeed " + s.bcm.speed;
    $("vcu").textContent =
      "ready " + s.vcu.ready + "\nmode " + s.vcu.mode + "\ntorque " + s.vcu.torque;
    $("bms").textContent =
      "V " + s.bms.pack_v + "\nSOC " + s.bms.soc + "%\nSOH " + s.bms.soh + "%\nT " + s.bms.tmin + "/" + s.bms.tmax + "\nIlim " + s.bms.lim_dch;
    $("tpms").textContent = "kPa " + s.tpms.join(" / ");
    $("can").textContent = s.can.map(function (f) {
      return f.id + "  " + f.data + "  " + f.name;
    }).join("\n");
    $("log").textContent = (s.log || []).join("\n");
    var tb = $("dtc");
    tb.innerHTML = "";
    (s.dtc || []).forEach(function (d) {
      var tr = document.createElement("tr");
      tr.innerHTML = "<td>" + d.code + "</td><td>" + d.text + "</td><td>" + d.status + "</td>";
      tb.appendChild(tr);
    });
    if (!$("products").dataset.ready) {
      s.products.forEach(function (p) {
        var b = document.createElement("button");
        b.type = "button";
        b.innerHTML = "<strong>" + p.id + "</strong><small>" + p.name + "</small>";
        b.addEventListener("click", function () {
          post("/api/product", { id: p.id }).then(render);
        });
        $("products").appendChild(b);
      });
      $("products").dataset.ready = "1";
    }
  }

  $("btn-virtual").addEventListener("click", function () {
    post("/api/mode", { mode: "virtual" }).then(render);
  });
  $("btn-hw").addEventListener("click", function () {
    post("/api/mode", { mode: "hardware" }).then(render);
  });
  $("btn-recover").addEventListener("click", function () {
    post("/api/recover", {}).then(render);
  });
  $("docs").href = "/docs/index.html";

  function poll() {
    fetch("/api/state").then(function (r) { return r.json(); }).then(render).catch(function () {});
  }
  poll();
  setInterval(poll, 400);
})();
