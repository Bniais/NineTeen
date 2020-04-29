<?php

  if($_SERVER['REQUEST_METHOD'] == 'POST')
  {
    include('include/secure.php');

    $gameId = $_POST["gameID"];
    $offset = $_POST["offset"];
    $limite = $_POST["limite"];

    $username = $_POST["username"];

    if($limite == "" )
    {
      $limite = 15;
    }

    if($offset == "")
    {
      $offset = 0;
    }
    // GLOBAL RECHERCHE
    if($gameId == "0")
    {
      if ( $resultat = $link->query("SELECT username, FLOOR( SUM( score * multiplicator ) ) AS total FROM nineteen_scores NATURAL JOIN nineteen_multiplicators NATURAL JOIN nineteen_players GROUP BY userId ORDER BY total DESC,username LIMIT $limite OFFSET $offset") )
        {
          while ($row = $resultat->fetch_assoc() ) {

            echo  $row["username"];
            echo " ";
            echo  $row["total"];
            echo " ";
          }
        }


    }
    else {
      if ( $resultat = $link->query("SELECT username,score FROM `nineteen_players` NATURAL JOIN nineteen_scores WHERE gameId = $gameId ORDER BY score DESC,username LIMIT $limite OFFSET $offset ") )
  		{
  			while ( $row = $resultat->fetch_assoc()) {
          echo  $row["username"];
          echo " ";
          echo  $row["score"];
          echo " ";
  			}
  		}
    }



    // RECHERCHE LE GAMEID CLASSEMENT

  }
?>
