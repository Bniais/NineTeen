<?php
    if( $_SERVER['REQUEST_METHOD'] == 'POST' ){
        $ping = $_POST["ping"];
        if($ping == "test" )
            echo "PONG";
    }
?>