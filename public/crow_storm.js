
// potentially intialize some stuff when the doc is ready
$(document).ready(function() {

	// real-time updates limit to 5 results
	$('#search-txt').keyup(function(){
        	searchCompanies(5);
	});


	// if they click search button, don't limit results
	$("#search-btn").click(function() {
		searchCompanies(0);
	});
})


function clearChart() {
	$("svg").first().remove();
}

function drawGraphHeader(cur_btn_name, name, symbol, symbols) {
	var output = '<ul class="searchresult">';
	var btn_list = [];
	
	output +='<li>';
	output +='<h2>' + name + '</h2>';
	output +='<p>';
	var symbol_list = symbols.split('|');
	
	$.each(symbol_list, function( index, value ) {
	  var btn_name = 'btn-' + value.replace("^","-").replace(".","-");
	  var btn_info = [btn_name, name, value, symbols];
	  
	  output += '<button id="'+ btn_name +'" name="' + btn_name + '" class="btn btn-secondary my-2 my-sm-0">' + value + '</button>';
	
	  btn_list.push(btn_info);
	});
	
	output += '</p>'
	output +='</li>';
	
	output += '</ul>';

	$('#update').html(output);
	
	// add button handlers here
	$.each(btn_list, function( index, btn_info ) {	 
		
		// if this is graph we are watching, set to "selected"
		if(btn_info[0] == cur_btn_name){
			$("#" + btn_info[0]).addClass('active');
		} 
		
		// keep click handler for ALL buttons in case we want to refresh
		$("#" + btn_info[0]).click(function() {
			drawGraph(btn_info[0], btn_info[1], btn_info[2], btn_info[3]);
		});
	});	
}


function chart(name, symbol, fullWidth, fullHeight) {

    var margin = {top: 20, right: 30, bottom: 30, left: 30},
            width = fullWidth - margin.left - margin.right,
            height = fullHeight - margin.top - margin.bottom,
            volumeHeight = fullHeight*.25;

    var parseDate = d3.timeParse("%Y-%m-%d");

    var zoom = d3.zoom()
            .on("zoom", zoomed);

    var x = techan.scale.financetime()
            .range([0, width]);

    var y = d3.scaleLinear()
            .range([height, 0]);

    var yPercent = y.copy();   // Same as y at this stage, will get a different domain later

    var yVolume = d3.scaleLinear()
            .range([height, height - volumeHeight]);

    var yInit, yPercentInit, zoomableInit;

    var candlestick = techan.plot.candlestick()
            .xScale(x)
            .yScale(y);

    var sma0 = techan.plot.sma()
            .xScale(x)
            .yScale(y);

    var sma1 = techan.plot.sma()
            .xScale(x)
            .yScale(y);

    var ema2 = techan.plot.ema()
            .xScale(x)
            .yScale(y);

    var volume = techan.plot.volume()
            .accessor(candlestick.accessor())   // Set the accessor to a ohlc accessor so we get highlighted bars
            .xScale(x)
            .yScale(yVolume);

    var xAxis = d3.axisBottom(x)
            .ticks(4);

    var yAxis = d3.axisRight(y)
            .ticks(4);

    var percentAxis = d3.axisLeft(yPercent)
            .ticks(4)
            .tickFormat(d3.format('+.1%'));

    var volumeAxis = d3.axisRight(yVolume)
            .ticks(2)
            .tickFormat(d3.format(",.3s"));

    var svg = d3.select("#chart").append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .append("g")
            .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

	d3.select("#chart").attr("align","center");

    svg.append("clipPath")
            .attr("id", "clip")
            .append("rect")
            .attr("x", 0)
            .attr("y", y(1))
            .attr("width", width)
            .attr("height", y(0) - y(1));

    svg.append('text')
            .attr("class", "symbol")
            .attr("x", 5)
            .text(name + " (" + symbol + ")");

    svg.append("g")
            .attr("class", "volume")
            .attr("clip-path", "url(#clip)");

    svg.append("g")
            .attr("class", "candlestick")
            .attr("clip-path", "url(#clip)");

    svg.append("g")
            .attr("class", "indicator sma ma-0")
            .attr("clip-path", "url(#clip)");

    svg.append("g")
            .attr("class", "indicator sma ma-1")
            .attr("clip-path", "url(#clip)");

    svg.append("g")
            .attr("class", "indicator ema ma-2")
            .attr("clip-path", "url(#clip)");

    svg.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + height + ")");

    svg.append("g")
            .attr("class", "y axis")
            .attr("transform", "translate(" + width + ",0)");

    svg.append("g")
            .attr("class", "percent axis");

    svg.append("g")
            .attr("class", "volume axis");

    svg.append("rect")
            .attr("class", "pane")
            .attr("width", width)
            .attr("height", height)
            .call(zoom);

    d3.csv("/symbol/csv/" + symbol, function (error, data) {
        var accessor = candlestick.accessor(),
            indicatorPreRoll = 1;  // Don't show where indicators don't have data

        data = data.map(function (d) {
            return {
                date: parseDate(d.Date),
                open: +d.Open,
                high: +d.High,
                low: +d.Low,
                close: +d.Close,
                volume: +d.Volume
            };
        }).sort(function (a, b) {
            return d3.ascending(accessor.d(a), accessor.d(b));
        });

        x.domain(techan.scale.plot.time(data, accessor).domain());
        y.domain(techan.scale.plot.ohlc(data.slice(indicatorPreRoll), accessor).domain());
        yPercent.domain(techan.scale.plot.percent(y, accessor(data[indicatorPreRoll])).domain());
        yVolume.domain(techan.scale.plot.volume(data, accessor.v).domain());

        svg.select("g.candlestick").datum(data).call(candlestick);
        svg.select("g.volume").datum(data).call(volume);
        svg.select("g.sma.ma-0").datum(techan.indicator.sma().period(10)(data)).call(sma0);
        svg.select("g.sma.ma-1").datum(techan.indicator.sma().period(20)(data)).call(sma1);
        svg.select("g.ema.ma-2").datum(techan.indicator.ema().period(50)(data)).call(ema2);


        zoomableInit = x.zoomable().domain([indicatorPreRoll, data.length]).copy(); // Zoom in a little to hide indicator preroll
        yInit = y.copy();
        yPercentInit = yPercent.copy();

        draw();
    });

    function reset() {
        zoom.scale(1);
        zoom.translate([0, 0]);
        draw();
    }

    function zoomed() {
        x.zoomable().domain(d3.event.transform.rescaleX(zoomableInit).domain());
        y.domain(d3.event.transform.rescaleY(yInit).domain());
        yPercent.domain(d3.event.transform.rescaleY(yPercentInit).domain());

        draw();
    }

    function draw() {
        svg.select("g.x.axis").call(xAxis);
        svg.select("g.y.axis").call(yAxis);
        svg.select("g.volume.axis").call(volumeAxis);
        svg.select("g.percent.axis").call(percentAxis);

        // We know the data does not change, a simple refresh that does not perform data joins will suffice.
        svg.select("g.candlestick").call(candlestick.refresh);
        svg.select("g.volume").call(volume.refresh);
        svg.select("g.sma.ma-0").call(sma0.refresh);
        svg.select("g.sma.ma-1").call(sma1.refresh);
        svg.select("g.ema.ma-2").call(ema2.refresh);
    }
}


function drawGraph(cur_btn_name, name, symbol, symbols) {

	$('#update').empty();
	drawGraphHeader(cur_btn_name, name, symbol, symbols);
	
    clearChart();
    
    chart(name, symbol, 960, 500);
}

function searchCompanies(searchLimit) {
        var search_field = $('#search-txt').val();
        var search_url = "/company/" + search_field + "/" + searchLimit;
        var btn_list = [];

        if(search_field == "") {
                var output = '<ul class="searchresult"></ul>';
                $('#update').html(output);
                return;
        }

        $.getJSON(search_url, function(data){
                var output = '<ul class="searchresult">';

        clearChart();

                $.each(data, function(key, val){
                        output +='<li>';
                        output +='<h2>' + key + '</h2>';
                        output +='<p>';
                        var symbols = val.split('|');

                        $.each(symbols, function( index, value ) {
                          var btn_name = 'btn-' + value.replace("^","-").replace(".","-");
                          var btn_info = [btn_name, key, value, val];

                          output += '<button id="'+ btn_name +'" name="' + btn_name + '" class="btn btn-secondary my-2 my-sm-0">' + value + '</button>';

                          btn_list.push(btn_info);
                        });

                        output += '</p>'
                        output +='</li>';
                });

                output += '</ul>';

                $('#update').html(output);

                // add button handlers here
                $.each(btn_list, function( index, btn_info ) {
                        $("#" + btn_info[0]).click(function() {
                                drawGraph(btn_info[0], btn_info[1], btn_info[2], btn_info[3]);
                        });
                });
        })
        .fail(function() {
            console.log( "error" );
        });
}
