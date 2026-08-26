(function () {
  var inPages = /\/pages\/|\\pages\\/.test(location.pathname);
  var p = inPages ? "" : "pages/";
  var root = inPages ? "../" : "";
  var items = [
    { href: root + "index.html", label: "Portal home", sec: "Start" },
    { href: p + "01-architecture.html", label: "01 Architecture", sec: "Documents" },
    { href: p + "02-requirements.html", label: "02 Requirements", sec: "Documents" },
    { href: p + "03-software-architecture.html", label: "03 Software architecture", sec: "Documents" },
    { href: p + "04-task-architecture.html", label: "04 Task architecture", sec: "Documents" },
    { href: p + "05-data-flow.html", label: "05 Data-flow", sec: "Documents" },
    { href: p + "06-control-flow.html", label: "06 Control-flow", sec: "Documents" },
    { href: p + "07-interfaces.html", label: "07 Interfaces", sec: "Documents" },
    { href: p + "08-repository-structure.html", label: "08 Repository structure", sec: "Documents" },
    { href: p + "09-roadmap.html", label: "09 Roadmap", sec: "Documents" },
    { href: p + "10-agent-breakdown.html", label: "10 Agent breakdown", sec: "Documents" },
    { href: p + "11-automotive-scenarios.html", label: "11 CAN scenarios", sec: "Flows" },
    { href: p + "12-sequential-flows.html", label: "12 Sequential flows", sec: "Flows" },
    { href: root + "presentation.html", label: "Presentation", sec: "Flows" }
  ];

  var here = location.pathname.replace(/\\/g, "/").split("/").pop();
  var html = '<p class="brand">AEGW-C3</p><h1>ESP32-C3 Automotive Platform</h1>';
  var lastSec = "";
  items.forEach(function (it) {
    if (it.sec !== lastSec) {
      html += '<div class="sec">' + it.sec + "</div>";
      lastSec = it.sec;
    }
    var name = it.href.split("/").pop();
    var cls = name === here ? ' class="active"' : "";
    html += '<a href="' + it.href + '"' + cls + ">" + it.label + "</a>";
  });
  document.getElementById("nav").innerHTML = html;
})();
