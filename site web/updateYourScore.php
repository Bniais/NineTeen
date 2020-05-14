<?php
include('include/fonction.php');

if( $_SERVER['REQUEST_METHOD'] == 'POST' ){

    $gameId = $_POST["gameID"];
    $score = $_POST["score"];
    $sessionKey = $_POST["key"];
    $secureKey = $_POST["secure"];

    if( $gameId != '' && $score != '' && $sessionKey != '' &&  $secureKey != '')
    {
        if( secureCheck($secureKey,2) == 1 )
        {
            include('include/secure.php');
            

            $result = $link->query( "UPDATE nineteen_scores 
            SET score = 
            CASE WHEN score < $score 
                THEN $score 
                ELSE score
            END 
            WHERE gameId = $gameId AND userId = (SELECT userId FROM nineteen_session WHERE sessionKey = '$sessionKey')");
            
            if($result)
            {
                echo "SUCCESS";
            }
            else
            {
                echo "-2"; // erreur connection
            }

            $link->close();
        }
        else
        {
            // requete invalide cle de securite incorrect
            echo "-10";
        }
    } 
    else 
    {
        echo "-1"; // champs non complet
        echo "\n";
        echo $gameId;
        echo "\n";
        echo $score;
        echo "\n";
        echo $sessionKey;
        echo "\n";
        echo $secureKey;
    }

}

?>