// <![CDATA[


	$(function() {

        //alert($("#myMenu li").html("TEST"));
        $("#myMenu li:has(ul)").prepend('<a href="#" class="expand"><div class="arrow"></div><div class="test"></div></a>').addClass("index_closed");
        $("#myMenu li:not(:has(ul))").addClass("index_item");
        $("#myMenu a.expand").click(function(event) {
            event.preventDefault();
            $(this).parent().children().filter("ul").toggle("slow");
            $(this).parent().toggleClass("index_closed");
            $(this).parent().toggleClass("index_open");
            });
        $("#myMenu li").children().filter("ul").hide().end();
        
        
	});


// ]]>