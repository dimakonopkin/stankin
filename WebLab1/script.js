function start ()
{

var url = "https://suggestions.dadata.ru/suggestions/api/4_1/rs/suggest/metro";
var token = "71bc63ba8c09f8ec980c8100ce422d3e13a2fe39";
var query = document.getElementById("vvod").value;

var options = {
    method: "POST",
    mode: "cors",
    headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        "Authorization": "Token " + token
    },
    body: JSON.stringify({query: query})
}


fetch(url, options)
  .then(function (response) {
    return response.json();
  })
  .then(function (data) {


                document.body.style.background = "#" + data.suggestions[0].data.color;
                if(data.suggestions[0].data.city == "Москва")
{
                document.getElementById("image1").src = "m1.png";
}
else
{
                document.getElementById("image1").src = "m2.png";
}

        	if (document.getElementById("check1").checked) 
    	{
    		document.getElementById("city").innerHTML = data.suggestions[0].data.city;
    		document.getElementById("city").style.display = "inline";

    	}
        else
    	{
    		document.getElementById("city").style.display = "none";
    	}

    	if (document.getElementById("check2").checked) 
    	{
    		document.getElementById("lname").innerHTML = data.suggestions[0].data.line_name;
    		document.getElementById("lname").style.display = "inline";
    	}
    	else
    	{
    		document.getElementById("name").style.display = "none";
    	}

    	if (document.getElementById("check3").checked) 
    	{
    		document.getElementById("lnum").innerHTML = data.suggestions[0].data.line_id;
    		document.getElementById("lnum").style.display = "inline";
    	}
    	else
    	{
    		document.getElementById("lnum").style.display = "none";
    	}
    	if (document.getElementById("check4").checked) 
    	{
    		document.getElementById("col").innerHTML = data.suggestions[0].data.color;
    		document.getElementById("col").style.display = "inline";
    	}
    	else
    	{
    		document.getElementById("col").style.display = "none";
    	}
  })
.catch(function(error){
console.log("error", error);
document.body.style.background = "#FFFFFF";
document.getElementById("image1").src = "e.png";

document.getElementById("city").style.display = "none";
document.getElementById("lname").style.display = "none";
document.getElementById("lnum").style.display = "none";
document.getElementById("col").style.display = "none";
});



}