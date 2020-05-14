-- phpMyAdmin SQL Dump
-- version OVH
-- https://www.phpmyadmin.net/
--
-- Hôte : -----------------------
-- Généré le :  jeu. 14 mai 2020 à 17:42
-- Version du serveur :  5.6.46-log
-- Version de PHP :  7.3.17

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de données :  `-----------`
--
CREATE DATABASE IF NOT EXISTS `-----------` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `------------`;

-- --------------------------------------------------------

--
-- Structure de la table `nineteen_players`
--

DROP TABLE IF EXISTS `nineteen_players`;
CREATE TABLE `nineteen_players` (
  `userId` int(11) NOT NULL,
  `username` varchar(32) NOT NULL,
  `password` varchar(256) NOT NULL,
  `email` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Index pour la table `nineteen_players`
--
ALTER TABLE `nineteen_players`
  ADD PRIMARY KEY (`userId`);

--
-- AUTO_INCREMENT pour la table `nineteen_players`
--
ALTER TABLE `nineteen_players`
  MODIFY `userId` int(11) NOT NULL AUTO_INCREMENT;
    
-- --------------------------------------------------------

--
-- Structure de la table `nineteen_session`
--

DROP TABLE IF EXISTS `nineteen_session`;
CREATE TABLE `nineteen_session` (
  `userId` int(11) NOT NULL REFERENCES (nineteen_players.userId),
  `sessionKey` varchar(256) NOT NULL,
  `sessionCreate` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `sessionLastLog` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Index pour la table `nineteen_session`
--
ALTER TABLE `nineteen_session`
  ADD PRIMARY KEY (`userId`,`sessionKey`);


-- --------------------------------------------------------

--
-- Structure de la table `nineteen_games`
--
DROP TABLE IF EXISTS `nineteen_games`;
CREATE TABLE `nineteen_games` (
  `gameId` int(11) NOT NULL DEFAULT '0',
  `name` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Index pour la table `nineteen_games`
--
ALTER TABLE `nineteen_games`
  ADD PRIMARY KEY (`gameId`);
  

  
-- --------------------------------------------------------

--
-- Structure de la table `nineteen_scores`
--

DROP TABLE IF EXISTS `nineteen_scores`;
CREATE TABLE `nineteen_scores` (
  `userId` int(11) NOT NULL DEFAULT '0' REFERENCES (nineteen_players.userId),
  `gameId` int(11) NOT NULL DEFAULT '0' REFERENCES (nineteen_games.gameId),
  `score` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Index pour la table `nineteen_scores`
--
ALTER TABLE `nineteen_scores`
  ADD PRIMARY KEY (`userId`,`gameId`);
  
-- --------------------------------------------------------

--
-- Structure de la table `nineteen_multiplicators`
--

DROP TABLE IF EXISTS `nineteen_multiplicators`;
CREATE TABLE `nineteen_multiplicators` (
  `gameId` int(11) NOT NULL REFERENCES (nineteen_games.gameId),
  `multiplicator` float DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Index pour la table `nineteen_multiplicators`
--
ALTER TABLE `nineteen_multiplicators`
  ADD PRIMARY KEY (`gameId`);

-- --------------------------------------------------------

--
-- Structure de la table `nineteen_tickets`
--

DROP TABLE IF EXISTS `nineteen_tickets`;
CREATE TABLE `nineteen_tickets` (
  `id` bigint(20) UNSIGNED NOT NULL,
  `statut` int(11) DEFAULT '0',
  `message` text,
  `ip` varchar(42) DEFAULT NULL,
  `date` timestamp NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Index pour la table `nineteen_tickets`
--
ALTER TABLE `nineteen_tickets`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id` (`id`);

--
-- AUTO_INCREMENT pour la table `nineteen_tickets`
--
ALTER TABLE `nineteen_tickets`
  MODIFY `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;
COMMIT;


/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
