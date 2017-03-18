
// when the page loads pop up a quick window
$(document).ready(function() {
	$("#alert-text").html("Crow storm intialized.");
})


$('#search-txt').keyup(function(){
	var search_field = $('#search-txt').val();
	var search_url = "/company/" + search_field + "/5";
	
	if(search_field == "") {
		var output = '<ul class="searchresult"></ul>';
		$('#update').html(output);
		return;
	}
	
	$.getJSON(search_url, function(data){
		var output = '<ul class="searchresult">';
		$.each(data, function(key, val){
			output +='<li>';
			output +='<h2>' + key + '</h2>';
			output +='<p>' + val + '</p>';
			output +='</li>';
		});
		
		output += '</ul>';
	
		$('#update').html(output);
	})
	.fail(function() {
	    console.log( "error" );
	});
});