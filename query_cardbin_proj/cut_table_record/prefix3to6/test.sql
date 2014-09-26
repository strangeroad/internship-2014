-- MySQL dump 10.10
--
-- Host: 10.12.194.206    Database: ia_gcms_2014
-- ------------------------------------------------------
-- Server version	5.5.19-log

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
-- Table structure for table `t_IP0040T1`
--

DROP TABLE IF EXISTS `t_IP0040T1`;
CREATE TABLE `t_IP0040T1` (
  `Feffective_time` varchar(10) NOT NULL DEFAULT '0',
  `Factive_inactive_code` varchar(1) NOT NULL DEFAULT '0',
  `Fissuer_acc_range_low` varchar(19) NOT NULL DEFAULT '0',
  `Fissuer_acc_range_prefix` varchar(4) NOT NULL DEFAULT '0',
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

--
-- Dumping data for table `t_IP0040T1`
--

-- 测试数据说明
-- 卡号0前导
-- 地位，高位的后几位不规则
-- 有;号

LOCK TABLES `t_IP0040T1` WRITE;
/*!40000 ALTER TABLE `t_IP0040T1` DISABLE KEYS */;
INSERT INTO `t_IP0040T1` VALUES 
('1328814','A','0424100000000000000','042','CIR','0424109999999999999','CIR','02','00000004782','1','0080255','USA','840','1','   ',' ','N','N','Y','CIR',' ','N','000000','840','2','0000000000000000000000000000',' ','000000','N','N','   ','000000','N','N','N','N','2014-08-28 17:05:10','2014-08-28 09:05:10'),
('1328814','A','3540080000888800099','354','CIR','3540489999999999900','CIR','02','00000003466','1','0080255','USA','840','1','   ',' ','N','N','N','CIR',' ','N','000000','840','2','0000000000000000000000000000',' ','000000','N','N','   ','000000','N','N','N','N','2014-08-28 17:05:11','2014-08-28 09:05:11'),
('1328814','A','0424100000600000000','042','CIR','0424109999999999999','CIR','02','00000004782','1','0080255','USA','840','1','   ',' ','N','N','Y','CIR',' ','N','000000','840','2','0000000000000000000000000000',' ','000000','N','N','   ','000000','N','N','N','N','2014-08-28 17:05:10','2014-08-28 09:05:10'),
('1328814','A','3552000000000000000','355','CIR','3552999999999999999','CIR','02','00000003466','1','0080255','USA','840','1','   ',' ','N','N','N','CIR',' ','N','000000','840','2','0000000000000000000000000000',' ','000000','N','N','   ','000000','N','N','N','N','2014-08-28 17:05:11','2014-08-28 09:05:11');



