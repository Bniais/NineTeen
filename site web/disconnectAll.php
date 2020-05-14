<?php
include('include/secure.php');
  if ($result = $link->query("DELETE FROM nineteen_session WHERE 1")) {
      echo "ALL USER ARE NOW DISCONNECTED";
  }
  else {
    echo "Une erreur est survenu";
  }
 ?>
