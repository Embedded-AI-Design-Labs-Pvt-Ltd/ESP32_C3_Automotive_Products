(function () {
  var state = null;
  var ucLoaded = false;

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

  function showTab(name) {
    document.querySelectorAll(".tabs button").forEach(function (b) {
      b.className = b.getAttribute("data-tab") === name ? "on" : "";
    });
    document.querySelectorAll(".panel").forEach(function (p) {
      p.className = "panel" + (p.id === "panel-" + name ? " on" : "");
    });
  }

  document.getElementById("tabs").addEventListener("click", function (e) {
    var t = e.target.getAttribute("data-tab");
    if (t) showTab(t);
  });

  function render(s) {
    state = s;
    $("soc").textContent = s.bms.soc;
    $("pack").textContent = s.bms.pack_v.toFixed(1);
    $("spd").textContent = s.bcm.speed;
    $("trq").textContent = s.vcu.torque;
    $("cantot").textContent = s.analyzer ? s.analyzer.total : "—";
    $("anunk").textContent = s.analyzer && s.analyzer.unknown ? ("unk " + s.analyzer.unknown) : "";
    $("fault").textContent = s.fault;
    $("fault").className = "fault-" + s.fault;
    $("sess").textContent = s.session;
    $("link").textContent = s.mode === "hardware"
      ? (s.hw_ok ? "ESP32-C3 linked" : "Live requested — Virtual ECU feed")
      : "Simulation · Virtual ECU";
    $("btn-virtual").className = s.mode === "virtual" ? "on" : "";
    $("btn-hw").className = s.mode === "hardware" ? "on" : "";
    $("bcm").textContent =
      "ign " + s.bcm.ign + "\ndoors " + s.bcm.doors + "\nlights 0x" + s.bcm.lights.toString(16) + "\nspeed " + s.bcm.speed;
    $("vcu").textContent =
      "ready " + s.vcu.ready + "\nmode " + s.vcu.mode + "\ntorque " + s.vcu.torque;
    $("bms").textContent =
      "V " + s.bms.pack_v + "\nSOC " + s.bms.soc + "%\nSOH " + s.bms.soh + "%\nT " + s.bms.tmin + "/" + s.bms.tmax + "\nIlim " + s.bms.lim_dch;
    $("tpms").textContent = "kPa " + s.tpms.join(" / ");
    $("can").textContent = (s.can || []).map(function (f) {
      return f.id + "  [" + f.dlc + "]  " + f.data + "  " + f.name;
    }).join("\n");
    $("log").textContent = (s.log || []).join("\n");
    var tb = $("dtc");
    tb.innerHTML = "";
    (s.dtc || []).forEach(function (d) {
      var tr = document.createElement("tr");
      tr.innerHTML = "<td>" + d.code + "</td><td>" + d.text + "</td><td>" + d.status + "</td>";
      tb.appendChild(tr);
    });
    if (s.uds) {
      $("uds-meta").textContent =
        "session " + s.uds.session +
        "\nunlocked " + s.uds.unlocked +
        "\nseed " + s.uds.seed +
        "\ncfg 0x" + s.uds.cfg;
      $("uds-io").textContent =
        "REQ  " + (s.uds.last_req || "—") + "\nRESP " + (s.uds.last_resp || "—");
    }
    if (s.analyzer) {
      $("an-top").textContent = (s.analyzer.top || []).map(function (r) {
        return r.id + "  ×" + r.count;
      }).join("\n") || "(empty)";
      $("an-sec").textContent =
        "unknown IDs " + s.analyzer.unknown +
        "\nsec events " + s.analyzer.sec_events +
        "\npause " + s.analyzer.pause +
        "\nfilter " + (s.analyzer.filter || "all");
    }
    $("uc-count").textContent = "(" + (s.usecase_count || 120) + ")";
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

  function loadUsecases() {
    var q = encodeURIComponent($("uc-q").value || "");
    var cat = encodeURIComponent($("uc-cat").value || "");
    fetch("/api/usecases?q=" + q + "&cat=" + cat)
      .then(function (r) { return r.json(); })
      .then(function (data) {
        var box = $("usecases");
        box.innerHTML = "";
        (data.usecases || []).forEach(function (u) {
          var b = document.createElement("button");
          b.type = "button";
          b.className = "uc";
          b.innerHTML = "<strong>UC-" + String(u.id).padStart(3, "0") + "</strong>" +
            "<span class='cat'>" + u.category + "</span>" +
            "<small>" + u.name + "</small>";
          b.addEventListener("click", function () {
            post("/api/usecase", { id: u.id }).then(function (s) {
              render(s);
              showTab("analyzer");
            });
          });
          box.appendChild(b);
        });
        ucLoaded = true;
      });
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

  $("uc-refresh").addEventListener("click", loadUsecases);
  $("uc-q").addEventListener("change", loadUsecases);
  $("uc-cat").addEventListener("change", loadUsecases);

  document.querySelectorAll("[data-uds]").forEach(function (btn) {
    btn.addEventListener("click", function () {
      var parts = btn.getAttribute("data-uds").trim().split(/\s+/);
      var sid = "0x" + parts[0];
      var data = parts.slice(1).join(" ");
      post("/api/uds", { sid: sid, data: data }).then(render);
    });
  });
  $("uds-send").addEventListener("click", function () {
    post("/api/uds", { sid: $("uds-sid").value, data: $("uds-data").value }).then(render);
  });

  $("an-pause").addEventListener("click", function () {
    post("/api/analyzer", { pause: true }).then(render);
  });
  $("an-resume").addEventListener("click", function () {
    post("/api/analyzer", { pause: false }).then(render);
  });
  $("an-clear").addEventListener("click", function () {
    post("/api/analyzer", { clear: true }).then(render);
  });
  $("an-apply").addEventListener("click", function () {
    var f = $("an-filter").value.trim() || "all";
    post("/api/analyzer", { filter: f }).then(render);
  });
  $("an-inject").addEventListener("click", function () {
    post("/api/analyzer", {
      inject: { id: $("inj-id").value, data: $("inj-data").value }
    }).then(render);
  });

  function poll() {
    fetch("/api/state").then(function (r) { return r.json(); }).then(function (s) {
      render(s);
      if (!ucLoaded) loadUsecases();
    }).catch(function () {});
  }
  poll();
  setInterval(poll, 400);
})();
