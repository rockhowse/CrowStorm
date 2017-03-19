
// potentially intialize some stuff when the doc is ready
$(document).ready(function() {
  
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
		
		// if this is graph we are watching, set to "selected" and no click handler
		if(btn_info[0] == cur_btn_name){
			$("#" + btn_info[0]).addClass('active');
		// otherwise, set the click handler so we can re-draw
		} else {
			$("#" + btn_info[0]).click(function() {
				drawGraph(btn_info[0], btn_info[1], btn_info[2], btn_info[3]);
			});
		}
	});	
}

function drawGraph(cur_btn_name, name, symbol, symbols) {

	$('#update').empty();
	drawGraphHeader(cur_btn_name, name, symbol, symbols);
	
    clearChart();
    
	var margin = {top: 20, right: 20, bottom: 30, left: 50},
            width = 960 - margin.left - margin.right,
            height = 500 - margin.top - margin.bottom;

    var parseDate = d3.timeParse("%Y-%m-%d");

    var x = techan.scale.financetime()
            .range([0, width]);

    var y = d3.scaleLinear()
            .range([height, 0]);

    var candlestick = techan.plot.candlestick()
            .xScale(x)
            .yScale(y);

    var xAxis = d3.axisBottom()
            .scale(x);

    var yAxis = d3.axisLeft()
            .scale(y);

    var svg = d3.select("#chart").append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .append("g")
            .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
            
	d3.select("#chart").attr("align","center");
	
    d3.csv("/symbol/csv/" + symbol, function(error, data) {
        var accessor = candlestick.accessor();

        data = data.slice(0, 200).map(function(d) {
            return {
                date: parseDate(d.Date),
                open: +d.Open,
                high: +d.High,
                low: +d.Low,
                close: +d.Close,
                volume: +d.Volume
            };
        }).sort(function(a, b) { return d3.ascending(accessor.d(a), accessor.d(b)); });

        svg.append("g")
                .attr("class", "candlestick");

        svg.append("g")
                .attr("class", "x axis")
                .attr("transform", "translate(0," + height + ")");

        svg.append("g")
                .attr("class", "y axis")
                .append("text")
                .attr("transform", "rotate(-90)")
                .attr("y", 6)
                .attr("dy", ".71em")
                .style("text-anchor", "end")
                .text("Price ($)");

        // Data to display initially
        draw(data.slice(0, data.length));
        // Only want this button to be active if the data has loaded
        d3.select("button").on("click", function() { draw(data); }).style("display", "inline");
    });

    function draw(data) {
        x.domain(data.map(candlestick.accessor().d));
        y.domain(techan.scale.plot.ohlc(data, candlestick.accessor()).domain());

        svg.selectAll("g.candlestick").datum(data).call(candlestick);
        svg.selectAll("g.x.axis").call(xAxis);
        svg.selectAll("g.y.axis").call(yAxis);
    }
}


$('#search-txt').keyup(function(){
	var search_field = $('#search-txt').val();
	var search_url = "/company/" + search_field + "/5";
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
});