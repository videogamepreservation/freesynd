var dt, ho, mi, se, per, tz, da, mo, ye, t;

ci1 = new Image(); ci1.src = "images/clock/1.png";
ci2 = new Image(); ci2.src = "images/clock/2.png";
ci3 = new Image(); ci3.src = "images/clock/3.png";
ci4 = new Image(); ci4.src = "images/clock/4.png";
ci5 = new Image(); ci5.src = "images/clock/5.png";
ci6 = new Image(); ci6.src = "images/clock/6.png";
ci7 = new Image(); ci7.src = "images/clock/7.png";
ci8 = new Image(); ci8.src = "images/clock/8.png";
ci9 = new Image(); ci9.src = "images/clock/9.png";
ci0 = new Image(); ci0.src = "images/clock/0.png";
cia = new Image(); cia.src = "images/clock/a.png";
cip = new Image(); cip.src = "images/clock/p.png";
cpl = new Image(); cpl.src = "images/clock/plus.png";
cmi = new Image(); cmi.src = "images/clock/minus.png";

function updateTimezone() {
    if (!document.images) return;

    dt = new Date();

    tz = -dt.getTimezoneOffset();

    // Timezone Offset
    if (tz < 0) {
        document.images.tzs.src = cmi.src;
        tz = -tz;
    } else {
        document.images.tzs.src = cpl.src;
    }

    t = tz / 60;
    document.images.tz3.src = eval("ci" + Math.floor(t / 10) + ".src");
    document.images.tz2.src = eval("ci" + (t % 10) + ".src");

    t = (tz % 60);
    document.images.tz1.src = eval("ci" + Math.floor(t / 10) + ".src");
    document.images.tz0.src = eval("ci" + (t % 10) + ".src");
}

function updateTime() {
    if (!document.images) return;

    dt = new Date();

    ho = dt.getHours();
    mi = dt.getMinutes();
    se = dt.getSeconds();

    per = "a";
    if (ho >= 12) {
        per = "p";
        ho = ho - 12;
    }
    if (ho == 0) ho = 12;

    // Hours
    document.images.ho1.src = eval("ci" + Math.floor(ho / 10) + ".src");
    document.images.ho0.src = eval("ci" + (ho % 10) + ".src");

    // Minutes
    document.images.mi1.src = eval("ci" + Math.floor(mi / 10) + ".src");
    document.images.mi0.src = eval("ci" + (mi % 10) + ".src");

    // Seconds
    document.images.se1.src = eval("ci" + Math.floor(se / 10) + ".src");
    document.images.se0.src = eval("ci" + (se % 10) + ".src");

    // AM/PM
    document.images.per.src = eval("ci" + per + ".src");

    setTimeout("updateTime()", 1000);
}

function updateDate() {
    if (!document.images) return;

    dt = new Date();

    da = dt.getDate();
    mo = dt.getMonth() + 1;
    ye = dt.getFullYear();

    // Year
    t = (ye % 100);
    document.images.ye1.src = eval("ci" + Math.floor(t / 10) + ".src");
    document.images.ye0.src = eval("ci" + (t % 10) + ".src");

    ye = ye / 100;
    document.images.ye3.src = eval("ci" + Math.floor(ye / 10) + ".src");
    document.images.ye2.src = eval("ci" + (ye % 10) + ".src");

    // Month
    document.images.mo1.src = eval("ci" + Math.floor(mo / 10) + ".src");
    document.images.mo0.src = eval("ci" + (mo % 10) + ".src");

    // Day
    document.images.da1.src = eval("ci" + Math.floor(da / 10) + ".src");
    document.images.da0.src = eval("ci" + (da % 10) + ".src");

    setTimeout("updateDate()", 1000);
}
