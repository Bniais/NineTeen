<?php
	
	$compteMessage = "S'inscrire à la beta Nineteen";

	if ( $_SERVER['REQUEST_METHOD'] == 'POST')
	{
		include('include/secure.php');

		$identifiant = $_POST["identifiant"];
		$password = $_POST["password"];
		$email = $_POST["email"];

		$passError = 0;
		if ( strlen( $password ) < 4)
		{
			$passError = 1;
			$passMsg = "4 caractères minimum ";
		}
		
		if ( !preg_match("/^[a-zA-Z0-9]*$/",$password) )
		{
			$passError = 1;
			$passMsg .= "Chiffre et lettre uniquement";
		}

		$emError = 0;
		if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
			$emError = 1;
			$emaMsg = "Format d'email incorrect";
		}

		$userError = 0;
		if (!preg_match("/^[a-zA-Z]*$/",$identifiant)) {
			$userError = 1;
			$userMsg = "Seules les lettres sont acceptées";
		}
		else
		{
			$result = $link->query("SELECT username FROM nineteen_players WHERE username = '$identifiant' ");
			if( $result->num_rows > 0 )
			{
				$userMsg = "Identifiant déjà utilisé";
				$userError = 1;

			}
		}


		if( !$passError && !$emError && !$userError)
		{
			$passMD5 = MD5($password);
			$result = $link->query("INSERT INTO `nineteen_players` ( `username` , `password` , `email` ) VALUES ('$identifiant','$passMD5','$email')");
			if( $result === TRUE)
			{

				$result = $link->query("SELECT `userId` FROM `nineteen_players` WHERE `username` = '$identifiant' ");
				if(  $result->num_rows == 1 )
				{
					while ($row = $result->fetch_assoc()) {
						$idUser =  $row["userId"];
					}

					for($i = 1; $i <= 15 ; $i++)
					{
						$result = $link->query("INSERT INTO `nineteen_scores` ( `userId` , `gameId` ) VALUES ($idUser,$i)");
					}

					$compteCree = 1;
					$compteMessage = "Inscription reussie";
				}
				else
				{
					printf("Message d'erreur : %s\n", $link->error);
					echo "Error get userId";
				}

			}
			else {
				printf("Message d'erreur : %s\n", $link->error);
				echo "Error";
			}
		}
		// SI AUCUNE ERR ON SEND REQUEST INSCRIPTION //
		$link->close();
	}
?>


<!DOCTYPE html>
<html>
	<?php 
	$titre = "Nineteen | Inscription";
	 include('header.php'); 
	?>
	
	<body>
		<?php include('navigation.php'); ?>
		<h4 class="title"><?php echo $compteMessage ?></h4>


		<?php if( $compteCree != 1) {?>
		<form action="inscription.php" method="POST" id="inscription" >
			<div class="omrs-input-group">
							<?php if ( $userError == 1 ) {?>

								<label class="omrs-input-underlined omrs-input-danger">
								<input required name="identifiant" value="<?php echo $identifiant ?>">
								<span class="omrs-input-label">Identifiant</span>
								<span class="omrs-input-helper"><?php echo $userMsg ?></span>

							<?php }else { ?>

								<label class="omrs-input-underlined">
								<input required name="identifiant" value="<?php echo $identifiant ?>">
								<span class="omrs-input-label">Identifiant</span>

							<?php } ?>


							<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24">
								<path fill="none" d="M0 0h24v24H0V0z"/><circle cx="15.5" cy="9.5" r="1.5"/><circle cx="8.5" cy="9.5" r="1.5"/><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm-5-6c.78 2.34 2.72 4 5 4s4.22-1.66 5-4H7z"/></svg>


							</label>
						</div>

						<div class="omrs-input-group">

						<?php if ( $passError == 1 ) {?>

							<label class="omrs-input-underlined omrs-input-danger">
							<input required type="password" name="password">
							<span class="omrs-input-label">Mot de passe</span>
							<span class="omrs-input-helper"><?php echo $passMsg ?></span>

						<?php }else { ?>

							<label class="omrs-input-underlined">
							<input required type="password" name="password">
							<span class="omrs-input-label">Mot de passe</span>

						<?php } ?>

							<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24">
								<path fill="none" d="M0 0h24v24H0V0z"/><circle cx="15.5" cy="9.5" r="1.5"/><circle cx="8.5" cy="9.5" r="1.5"/><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm-5-6c.78 2.34 2.72 4 5 4s4.22-1.66 5-4H7z"/></svg>


							</label>
						</div>

						<div class="omrs-input-group ">

							<?php if ( $emError == 1 ) {?>

								<label class="omrs-input-underlined omrs-input-danger">
								<input required name="email" value="<?php echo $email ?>">
								<span class="omrs-input-label">Email</span>
								<span class="omrs-input-helper"><?php echo $emaMsg ?></span>

							<?php }else { ?>

								<label class="omrs-input-underlined">
								<input required name="email" value="<?php echo $email ?>">
								<span class="omrs-input-label">Email</span>

							<?php } ?>

							<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24">
								<path fill="none" d="M0 0h24v24H0V0z"/><circle cx="15.5" cy="9.5" r="1.5"/><circle cx="8.5" cy="9.5" r="1.5"/><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm-5-6c.78 2.34 2.72 4 5 4s4.22-1.66 5-4H7z"/></svg>


							</label>
						</div>


		</form>
		<button class="button" style="vertical-align:middle" type="submit" form="inscription" value="Submit"><span>S'inscrire</span></button>

		<?php } ?>
		<br>
		<br>
		<br>

	</body>
</html>
