-- MySQL dump 10.13  Distrib 5.5.37, for debian-linux-gnu (i686)
--
-- Host: localhost    Database: ia_gcms_2014
-- ------------------------------------------------------
-- Server version	5.5.37-0ubuntu0.14.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `t_IP0040T1_prefix3to6`
--

DROP TABLE IF EXISTS `t_IP0040T1_prefix3to6`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_IP0040T1_prefix3to6` (
  `Feffective_time` varchar(10) NOT NULL DEFAULT '0',
  `Factive_inactive_code` varchar(1) NOT NULL DEFAULT '0',
  `Fissuer_acc_range_low` varchar(19) NOT NULL DEFAULT '0',
  `Fissuer_acc_range_prefix` varchar(8) NOT NULL DEFAULT '0',
  `Fgcms_product_id` varchar(3) NOT NULL DEFAULT '0',
  `Fissuer_acc_range_high` varchar(19) NOT NULL DEFAULT '0',
  `Fcard_program_id` varchar(3) NOT NULL DEFAULT '0',
  `Fissuer_card_prog_id` varchar(2) NOT NULL DEFAULT '0',
  `Fnumber_id` varchar(11) NOT NULL DEFAULT '0',
  `Fproduct_type_id` varchar(1) NOT NULL DEFAULT '0',
  `Fendpoint` varchar(7) NOT NULL DEFAULT '0',
  `Fcountry_code_alpha` varchar(3) NOT NULL DEFAULT '0',
  `Fcountry_code_num` varchar(3) NOT NULL DEFAULT '0',
  `Fregion` varchar(1) NOT NULL DEFAULT '0',
  `Fproduct_class` varchar(3) NOT NULL DEFAULT '0',
  `Ftrans_routing_id` varchar(1) NOT NULL DEFAULT '0',
  `Fpresent_reassign_switch` varchar(1) NOT NULL DEFAULT '0',
  `Fproduct_reassign_switch` varchar(1) NOT NULL DEFAULT '0',
  `Fpwcb_optin_switch` varchar(1) NOT NULL DEFAULT '0',
  `Flicensed_product_id` varchar(3) NOT NULL DEFAULT '0',
  `Fmap_service_id` varchar(1) NOT NULL DEFAULT '0',
  `Facc_level_man_part_id` varchar(1) NOT NULL DEFAULT '0',
  `Facc_level_man_active_date` varchar(6) NOT NULL DEFAULT '0',
  `Fcardholder_bill_cur_def` varchar(3) NOT NULL DEFAULT '0',
  `Fcardholder_bill_cur_exp_def` varchar(1) NOT NULL DEFAULT '0',
  `Fcardholder_bill_pri_cur` varchar(28) NOT NULL DEFAULT '0',
  `Fchip_to_mag_conv_sev_id` varchar(1) NOT NULL DEFAULT '0',
  `Ffloor_exp_date` varchar(6) NOT NULL DEFAULT '0',
  `Fcobrand_part_switch` varchar(1) NOT NULL DEFAULT '0',
  `Fspend_con_switch` varchar(1) NOT NULL DEFAULT '0',
  `Fmer_clean_sev_part` varchar(3) NOT NULL DEFAULT '0',
  `Fmer_clean_act_date` varchar(6) NOT NULL DEFAULT '0',
  `Fpaypass_enable_id` varchar(1) NOT NULL DEFAULT '0',
  `Freg_rate_type_id` varchar(1) NOT NULL DEFAULT '0',
  `Fpsn_route_id` varchar(1) NOT NULL DEFAULT '0',
  `Fcashback_without_pur_id` varchar(1) NOT NULL DEFAULT '0',
  `Fcreate_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `Fmodify_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Fissuer_acc_range_low`,`Fgcms_product_id`),
  KEY `Fissuer_acc_range_prefix` (`Fissuer_acc_range_prefix`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_IP0040T1_prefix3to6`
--

LOCK TABLES `t_IP0040T1_prefix3to6` WRITE;
/*!40000 ALTER TABLE `t_IP0040T1_prefix3to6` DISABLE KEYS */;
INSERT INTO `t_IP0040T1_prefix3to6` VALUES 
