<?php

	if( $_SERVER['REQUEST_METHOD'] == 'POST' )
	{
		$versionActuel = "0.1.4b";
		if ( $_POST["version"] == $versionActuel )
			echo "1";
		else 
			echo "0";
			
	}
?>