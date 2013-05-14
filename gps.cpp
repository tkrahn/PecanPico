/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "config.h"
#include "gps.h"


#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>


PROGMEM const prog_uchar ubx_setNav[] = {
  0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 
  0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 
  0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x16, 0xDC 
};

// The ISS Kepplerian Elements calculated dataset (starting at Apr. 8th 23:00:00 UTC)
PROGMEM const prog_uint16_t ISSData[] = {
	26392, 24859, 23325, 21792, 20258, 18725, 17192, 16171, 
	14638, 13618, 12085, 11065, 10046, 9026, 8519, 8013, 
	7506, 7000, 6494, 6500, 6146, 6663, 7181, 7699, 
	8216, 9245, 10273, 11301, 12329, 13357, 14896, 16436, 
	17463, 19001, 20540, 22079, 23617, 25156, 26694, 28232, 
	29770, 31309, 32847, 34385, 35923, 37461, 38999, 40538, 
	42076, 43615, 45153, 46692, 47719, 49258, 50797, 51824, 
	53364, 54392, 55420, 56449, 56965, 57995, 58512, 59030, 
	59548, 59554, 59560, 59053, 59059, 58553, 57534, 57027, 
	56008, 54988, 53968, 52436, 51416, 49883, 48862, 47329, 
	45796, 44262, 42729, 41195, 39662, 38128, 36594, 35060, 
	33526, 31993, 30459, 28925, 27391, 25857, 24324, 22790, 
	21257, 19723, 18190, 17169, 15636, 14616, 13083, 12063, 
	11043, 10023, 9004, 8497, 7479, 6972, 6978, 6472, 
	6478, 6996, 7002, 7519, 8037, 8706, 9222, 10251, 
	11279, 12819, 13846, 15386, 16413, 17952, 19491, 21029, 
	22056, 23594, 25133, 26671, 28209, 29747, 31286, 33336, 
	34874, 36412, 37950, 39489, 41027, 42565, 44104, 45642, 
	46669, 48208, 49747, 50774, 52314, 53341, 54369, 55398, 
	56426, 57455, 57972, 58490, 59008, 59526, 59532, 59538, 
	59032, 58525, 58019, 57512, 56493, 55474, 54454, 53434, 
	52414, 50881, 49860, 48327, 46794, 45261, 43727, 42194, 
	40660, 39127, 37593, 36059, 34525, 32991, 31458, 29924, 
	28390, 26856, 25322, 23789, 22255, 20722, 19189, 18167, 
	16634, 15102, 14081, 12549, 11529, 10509, 9489, 8982, 
	7963, 7457, 6950, 6444, 6450, 6456, 6974, 6980, 
	7497, 8526, 9043, 10072, 11100, 12129, 13156, 14696, 
	15363, 16902, 18441, 19980, 21007, 22545, 24083, 25622, 
	27160, 28698, 30236, 31775, 33313, 34851, 36901, 38439, 
	39978, 41516, 43054, 44081, 45619, 47158, 48697, 50236, 
	51264, 52803, 53831, 54859, 55887, 56916, 57433, 58462, 
	58980, 58986, 59504, 59510, 59516, 59010, 58503, 57997, 
	56978, 56471, 55451, 54431, 52899, 51879, 50346, 49326, 
	47793, 46259, 44726, 43705, 42171, 40637, 39104, 37570, 
	35524, 33990, 32456, 30923, 29389, 27855, 26321, 24788, 
	23254, 21720, 20187, 19166, 17633, 16100, 15079, 13546, 
	12526, 11506, 10486, 9467, 8448, 7941, 7435, 6928, 
	6422, 6428, 6434, 6952, 7470, 7987, 8504, 9533, 
	10562, 11590, 12618, 13646, 15185, 16212, 17752, 19290, 
	20317, 21856, 23394, 24933, 26471, 27649, 29187, 30726, 
	32264, 33802, 35340, 36878, 38416, 39955, 41493, 43031, 
	44570, 46109, 47647, 49186, 50214, 51753, 52781, 53809, 
	55349, 56377, 56894, 57923, 58440, 58958, 58964, 59482, 
	59488, 58982, 58988, 58481, 57463, 56956, 55936, 54917, 
	53897, 52877, 51344, 50324, 48791, 47258, 45725, 44703, 
	43170, 41636, 40102, 38569, 37035, 35501, 33967, 31922, 
	30388, 28854, 27320, 25786, 24253, 22719, 21186, 20164, 
	18631, 17098, 15565, 14544, 13012, 11992, 10972, 9952, 
	8933, 8426, 7407, 6901, 6906, 6400, 6406, 6412, 
	6930, 7448, 7965, 8994, 9511, 10539, 11568, 12596, 
	14135, 15163, 16702, 18241, 19268, 20806, 22345, 23883, 
	25422, 26960, 28498, 30036, 31575, 33113, 34651, 36189, 
	37727, 39265, 40804, 42342, 43521, 45059, 46598, 48137, 
	49164, 50703, 51730, 53270, 54298, 55326, 56355, 57384, 
	57901, 58418, 58936, 59454, 59460, 59466, 58960, 58454, 
	57947, 57441, 56422, 55914, 54895, 53363, 52342, 51322, 
	49789, 48256, 47235, 45702, 44168, 42635, 41101, 39568, 
	38034, 36500, 34966, 33432, 31899, 30365, 28319, 26785, 
	25251, 23718, 22696, 21163, 19629, 18096, 16563, 15542, 
	14010, 12989, 11969, 10437, 9930, 8911, 7892, 7385, 
	6879, 6884, 6378, 6384, 6902, 6908, 7426, 7943, 
	8972, 10001, 11029, 12057, 13085, 14113, 15652, 17191, 
	18218, 19757, 21295, 22834, 24372, 25911, 27449, 28987, 
	30525, 32064, 33602, 35140, 36678, 38216, 39755, 41293, 
	42831, 44370, 45908, 47447, 48474, 50013, 51552, 52580, 
	54120, 54788, 55816, 56845, 57362, 57879, 58397, 58914, 
	59432, 59438, 59444, 58938, 58432, 57925, 57418, 56399, 
	55380, 54360, 53340, 51808, 50787, 49254, 47721, 46700, 
	45167, 43634, 42100, 40566, 39033, 37499, 35965, 34431, 
	32897, 31364, 29830, 28296, 26762, 25229, 23695, 22161, 
	20628, 19095, 17562, 16541, 15008, 13475, 12455, 11435, 
	10415, 9396, 8376, 7870, 7363, 6857, 6351, 6357, 
	6362, 6880, 7398, 7916, 8433, 8950, 9978, 11007, 
	12035, 13575, 14602, 16141, 17168, 18707, 20246, 21785, 
	23323, 24862, 26400, 27938, 29476, 31014, 32553, 34091, 
	35629, 37167, 38705, 40244, 41782, 43320, 44859, 46398, 
	47936, 48963, 50502, 51530, 53069, 54097, 55125, 56154, 
	57183, 57700, 58369, 58887, 58892, 59410, 59416, 58910, 
	58916, 58410, 57903, 56884, 55865, 54846, 53826, 52806, 
	51785, 50253, 48720, 47699, 46165, 44632, 43099, 41565, 
	40031, 38498, 36964, 35430, 33896, 32363, 30829, 29295, 
	27761, 26227, 24694, 23160, 21627, 20093, 18560, 17027, 
	16006, 14473, 13453, 11920, 10900, 9881, 9373, 8354, 
	7848, 7341, 6835, 6329, 6335, 6341, 6858, 7376, 
	7894, 8411, 9440, 10468, 11496, 12524, 13552, 15091, 
	16119, 17658, 19197, 20735, 22274, 23812, 24839, 26377, 
	28427, 29965, 31504, 33042, 34580, 36118, 37656, 39194, 
	40733, 42271, 43810, 45348, 46887, 47914, 49453, 50992, 
	52019, 53559, 54587, 55615, 56644, 57160, 58190, 58707, 
	59225, 59743, 59749, 59395, 58888, 58894, 58388, 57369, 
	56862, 55843, 54823, 53803, 52271, 51251, 49718, 48697, 
	47164, 45631, 44097, 42564, 41030, 39497, 37963, 36429, 
	34895, 33361, 31828, 30294, 28760, 27226, 25692, 24159, 
	22625, 21092, 19558, 18025, 17004, 15471, 14451, 12918, 
	11898, 10878, 9858, 8839, 8332, 7314, 6807, 6813, 
	6307, 6313, 6831, 6837, 7354, 7872, 8901, 9417, 
	10446, 11474, 13014, 14041, 15581, 16608, 18147, 19686, 
	21224, 22251, 23789, 25328, 26866, 28404, 29942, 31481, 
	33019, 35069, 36607, 38145, 39684, 41222, 42760, 44299, 
	45325, 46864, 48403, 49942, 50969, 52509, 53536, 54564, 
	55593, 56621, 57650, 58167, 58685, 59203, 59721, 59727, 
	59733, 59226, 58720, 58214, 57347, 56328, 55308, 54289, 
	53269, 52248, 50716, 49695, 48162, 46629, 45096, 43562, 
	42029, 41007, 39474, 37428, 35894, 34360, 32826, 31293, 
	29759, 28225, 26691, 25157, 23624, 22090, 20557, 19024, 
	18002, 16469, 14937, 13916, 12384, 11363, 10344, 9324, 
	8817, 7798, 7291, 6785, 6279, 6285, 6291, 6809, 
	6815, 7332, 8361, 8878, 9907, 10935, 11963, 12991, 
	14531, 15558, 17097, 18636, 19663, 21202, 22740, 24278, 
	25817, 27355, 28893, 30431, 31970, 33508, 35046, 36584, 
	38122, 40173, 41711, 42737, 44276, 45814, 47353, 48892, 
	49919, 51459, 52486, 54026, 55054, 56082, 57111, 57628, 
	58657, 59175, 59181, 59699, 59705, 59711, 59205, 58698, 
	58192, 57173, 56666, 55646, 54626, 53094, 51714, 50181, 
	49161, 47628, 46094, 45073, 43540, 42006, 40472, 38939, 
	37405, 35871, 33825, 32291, 30758, 29224, 27690, 26156, 
	24623, 23089, 21555, 20022, 19001, 17468, 15935, 14914, 
	13381, 12361, 11341, 10321, 9302, 8283, 7776, 7269, 
	6763, 6257, 6263, 6269, 6787, 7305, 7822, 8339, 
	9368, 10397, 11425, 12453, 13481, 14508, 16047, 17587, 
	18613, 20152, 21691, 23229, 24768, 26306, 27844, 29382, 
	30921, 32459, 33997, 35535, 37073, 38611, 40150, 41688, 
	43226, 44765, 46304, 47842, 49381, 50409, 51948, 52976, 
	54003, 55544, 56060, 57089, 58118, 58635, 59153, 59159, 
	59677, 59683, 59177, 59183, 58676, 57658, 57151, 56131, 
	55112, 54092, 53072, 51539, 50519, 48986, 47453, 46432, 
	44898, 43365, 41831, 39938, 38404, 36870, 35336, 33802, 
	32269, 30223, 28689, 27155, 25621, 24088, 22554, 21533, 
	19999, 18466, 16933, 15400, 14379, 12847, 11827, 10807, 
	9787, 8768, 8261, 7242, 6736, 6741, 6235, 6241, 
	6247, 6765, 7283, 7800, 8829, 9346, 10374, 11403, 
	12431, 13970, 14998, 16537, 17564, 19103, 20641, 22180, 
	23718, 25257, 26795, 28333, 29871, 31410, 32948, 34486, 
	36024, 37562, 39100, 40639, 42177, 43716, 45254, 46793, 
	48332, 49359, 50898, 51925, 53465, 54493, 55521, 56550, 
	57579, 58096, 58613, 59131, 59649, 59655, 59661, 59155, 
	58649, 58142, 57636, 56617, 56109, 55089, 53558, 52537, 
	51517, 49984, 48451, 47430, 45897, 44363, 42830, 41296, 
	39763, 38229, 36695, 35161, 33627, 32094, 30560, 29026, 
	27492, 25958, 24065, 22531, 20998, 19464, 17931, 16398, 
	15377, 13845, 12824, 11804, 10272, 9765, 8746, 7727, 
	7220, 6714, 6719, 6213, 6219, 6737, 6743, 7261, 
	7778, 8807, 9836, 10864, 11892, 12920, 13948, 15487, 
	17026, 18053, 19592, 21130, 22669, 24207, 25746, 27284, 
	28822, 30360, 31899, 33437, 34975, 36513, 38051, 39590, 
	41128, 42666, 44205, 45743, 47282, 48309, 49848, 51387, 
	52415, 53443, 54983, 56011, 56528, 57557, 58074, 58591, 
	59109, 59627, 59633, 59639, 59133, 58627, 58120, 57613, 
	56594, 55575, 54555, 53535, 52003, 50982, 49449, 48428, 
	46895, 45362, 43829, 42295, 40761, 39228, 37694, 36160, 
	34626, 33092, 31559, 30025, 28491, 26957, 25424, 23890, 
	22356, 20823, 19290, 17757, 16736, 15203, 14182, 12290, 
	11270, 10250, 9231, 8211, 7705, 7198, 6692, 6186, 
	6191, 6197, 6715, 6721, 7751, 8268, 8785, 9813, 
	10842, 11870, 13410, 14437, 15976, 17003, 18542, 20081, 
	21620, 23158, 24697, 26235, 27773, 29311, 30850, 32388, 
	33926, 35464, 37002, 38540, 40079, 41617, 43155, 44694, 
	46233, 47259, 48798, 50337, 51365, 52904, 53932, 54960, 
	55989, 57017, 57535, 58564, 59082, 59087, 59605, 59611, 
	59105, 59111, 58605, 58098, 57079, 56060, 55041, 54021, 
	53001, 51980, 50448, 48915, 47894, 46360, 44827, 43294, 
	41760, 40226, 38693, 37159, 35625, 34091, 32558, 31024, 
	29490, 27956, 26422, 24889, 23355, 21822, 20288, 18755, 
	17734, 16201, 14668, 13648, 12627, 11095, 10076, 9568, 
	8549, 7683, 7176, 6670, 6164, 6170, 6176, 6693, 
	7211, 7729, 8246, 9275, 10303, 11331, 12359, 13387, 
	14926, 15954, 17493, 19032, 20570, 22109, 23135, 24674, 
	26212, 27750, 29288, 31339, 32877, 34415, 35953, 37491, 
	39029, 40568, 42106, 43645, 45183, 46210, 47749, 49288, 
	50827, 51854, 52882, 54422, 55450, 56479, 56995, 58024, 
	58542, 59060, 59577, 59583, 59589, 59083, 59089, 58583, 
	57564, 57057, 56038, 55018, 53998, 52466, 51446, 49913, 
	48892, 47359, 45826, 44292, 42759, 41225, 39692, 38158, 
	36624, 35090, 33556, 32023, 30489, 28955, 27421, 25887, 
	24354, 22820, 21287, 19753, 18220, 17199, 15666, 14646, 
	13113, 12093, 11073, 10053, 9034, 8527, 7508, 7002, 
	7008, 6502, 6148, 6666, 6671, 7189, 7706, 8736, 
	9252, 10281, 11309, 12849, 13876, 15416, 16443, 17982, 
	19521, 20547, 22086, 23624, 25163, 26701, 28239, 29777, 
	31316, 32854, 34904, 36442, 37980, 39519, 41057, 42595, 
	44134, 45160, 46699, 48238, 49777, 50804, 52344, 53371, 
	54399, 55428, 56456, 57485, 58002, 58520, 59038, 59556, 
	59562, 59568, 59062, 58555, 58049, 57542, 56523, 55504, 
	54484, 53464, 52444, 50911, 49890, 48357, 46824, 45291, 
	43757, 42736, 41202, 39669, 37623, 36089, 34555, 33021, 
	31488, 29954, 28420, 26886, 25353, 23819, 22285, 20752, 
	19219, 18197, 16664, 15132, 14111, 12579, 11559, 10539, 
	9519, 9012, 7993, 7486, 6980, 6474, 6480, 6486, 
	7004, 7010, 7527, 8196, 8713, 9742, 10770, 11799, 
	12826, 14366, 15393, 16932, 18471, 19498, 21037, 22575, 
	24114, 25652, 27190, 28728, 30267, 31805, 33343, 34881, 
	36419, 37957, 40008, 41034, 42572, 44111, 45650, 47188, 
	48727, 49754, 51294, 52321, 53861, 54889, 55917, 56946, 
	57463, 58492, 59010, 59016, 59534, 59540, 59546, 59040, 
	58533, 58027, 57008, 56501, 55481, 54461, 52929, 51909, 
	50376, 49356, 47823, 46289, 45268, 43735, 42201, 40667, 
	39134, 37600, 36066, 34532, 32487, 30953, 29419, 27885, 
	26351, 24818, 23284, 21751, 20217, 19196, 17663, 16130, 
	15109, 13576, 12556, 11536, 10516, 9497, 8478, 7971, 
	7465, 6958, 6452, 6458, 6464, 6982, 7500, 8017, 
	8534, 9563, 10080, 11620, 12288, 13316, 14343, 15883, 
	17422, 18448, 19987, 21526, 23064, 24603, 26141, 27679, 
	29217, 30756, 32294, 33832, 35370, 36908, 38446, 39985, 
	41523, 43062, 44600, 46139, 47678, 49217, 50244, 51783, 
	52811, 53839, 55379, 55895, 56924, 57953, 58470, 58988, 
	58994, 59512, 59518, 59012, 59018, 58511, 57493, 56986, 
	55966, 54947, 53927, 52907, 51374, 50354, 48821, 47288, 
	46267, 44733, 43200, 41666, 40133, 38599, 37065, 35531, 
	33997, 32464, 30930, 28884, 27350, 25816, 24283, 22749, 
	21728, 20194, 18661, 17128, 15595, 14574, 13554, 12022, 
	11002, 9982, 8963, 8456, 7437, 6931, 6936, 6430, 
	6436, 6442, 6960, 7478, 7995, 9024, 9541, 10569, 
	11598, 12626, 14165, 15193, 16732, 17759, 19298, 20836, 
	22375, 23553, 25092, 26630, 28168, 29706, 31245, 32783, 
	34321, 35859, 37397, 38936, 40474, 42012, 43551, 45089, 
	46628, 48167, 49194, 50733, 51760, 53300, 54328, 55356, 
	56385, 57414, 57931, 58448, 58966, 59484, 59490, 59496, 
	58990, 58484, 57977, 57471, 56452, 55944, 54925, 53393, 
	52372, 51352, 49819, 48286, 47265, 45732, 44198, 42665, 
	41131, 39598, 38064, 36530, 34996, 33462, 31929, 30395, 
	28861, 27327, 25794, 24260, 22726, 21193, 19660, 18126, 
	16593, 15572, 14040, 13019, 11999, 10467, 9960, 8941, 
	7922, 7415, 6909, 6914, 6408, 6414, 6932, 6938, 
	7456, 7973, 9002, 10031, 11059, 12087, 13115, 14143, 
	15682, 17221, 18248, 19787, 21326, 22864, 24402, 25941, 
	27479, 29017, 30556, 32094, 33632, 35170, 36708, 38246, 
	39425, 40963, 42501, 44040, 45578, 47117, 48144, 49683, 
	51222, 52250, 53278, 54818, 55846, 56363, 57392, 57909, 
	58426, 58944, 59462, 59468, 59474, 58968, 58462, 57955, 
	57448, 56429, 55410, 54390, 53370, 51838, 50817, 49284, 
	48263, 46730, 45197, 43664, 42130, 40596, 39063, 37529, 
	35995, 34461, 32928, 31394, 29860, 28326, 26792, 25259, 
	23725, 22191, 20658, 19125, 17592, 16571, 15038, 14017, 
	12485, 11465, 10445, 9426, 8406, 7900, 7393, 6887, 
	6381, 6387, 6392, 6910, 6916, 7946, 8463, 8980, 
	10008, 11037, 12065, 13605, 14632, 16171, 17198, 18737, 
	20276, 21815, 23353, 24892, 26430, 27968, 29506, 31045, 
	32583, 34121, 35659, 37197, 38735, 40274, 41812, 43350, 
	44889, 46428, 47454, 48993, 50533, 51560, 52739, 53767, 
	54795, 55824, 56853, 57370, 58399, 58917, 58922, 59440, 
	59446, 58940, 58946, 58440, 57933, 56914, 55895, 54876, 
	53856, 52836, 51815, 50283, 48750, 47729, 46196, 44662, 
	43129, 41595, 40062, 38528, 36994, 35460, 33926, 32393, 
	30859, 29325, 27791, 26257, 24724, 23190, 21657, 20123, 
	18590, 17569, 16036, 14503, 13483, 12462, 10930, 9911, 
	9403, 8384, 7878, 7371, 6865, 6359, 6365, 6371, 
	6888, 7406, 7924, 8441, 9470, 10498, 11526, 12554, 
	13582, 15121, 16149, 17688, 19227, 20765, 22304, 23330, 
	24869, 26407, 27945, 29483, 31534, 33072, 34610, 36148, 
	37686, 39225, 40763, 42301, 43840, 45378, 46405, 47944, 
	49483, 51022, 52049, 53077, 54617, 55645, 56674, 57190, 
	57860, 58377, 58895, 59413, 59419, 59425, 58918, 58924, 
	58418, 57399, 56892, 55873, 54853, 53833, 52301, 51281, 
	49748, 48727, 47194, 45661, 44127, 42594, 41060, 39527, 
	37993, 36459, 34925, 33391, 31858, 30324, 28790, 27256, 
	25722, 24189, 22655, 21122, 19588, 18055, 17034, 15501, 
	14481, 12948, 11928, 10908, 9889, 8869, 8362, 7344, 
	6837, 6843, 6337, 6343, 6861, 6867, 7384, 7902, 
	8931, 9447, 10476, 11504, 13044, 14072, 15611, 16638, 
	18177, 19716, 20742, 22281, 23819, 25358, 26896, 28434, 
	29973, 31511, 33049, 35099, 36637, 38175, 39714, 41252, 
	42790, 44329, 45355, 46894, 48433, 49972, 50999, 52539, 
	53566, 54594, 55623, 56651, 57680, 58197, 58715, 59233, 
	59751, 59397, 59403, 58897, 58390, 57884, 57377, 56358, 
	55339, 54319, 53299, 52279, 50746, 49725, 48192, 46659, 
	45126, 43593, 42571, 41037, 39504, 37458, 35924, 34390, 
	32857, 31323, 29789, 28255, 26721, 25188, 23654, 22120, 
	20587, 19054, 18032, 16499, 14967, 13946, 12414, 11394, 
	10374, 9354, 8847, 7828, 7322, 6815, 6309, 6315, 
	6321, 6839, 6845, 7362, 8391, 8908, 9937, 10966, 
	11994, 13021, 14561, 15588, 17127, 18666, 19693, 21232, 
	22770, 24309, 25847, 27385, 28923, 30462, 32000, 33538, 
	35076, 36614, 38152, 39691, 41229, 42767, 44306, 45845, 
	47383, 48922, 49949, 51489, 52516, 54056, 55084, 56112, 
	57141, 57658, 58687, 59205, 59211, 59729, 59735, 59741, 
	59235, 58368, 57862, 56843, 56336, 55316, 54296, 52764, 
	51744, 50723, 49191, 47658, 46124, 45103, 43570, 42036, 
	40503, 38969, 37435, 35901, 34367, 32322, 30788, 29254, 
	27720, 26186, 24653, 23119, 21586, 20052, 19031, 17498, 
	15965, 14944, 13411, 12391, 11371, 10351, 9332, 8313, 
	7806, 7300, 6793, 6287, 6293, 6299, 6817, 7335, 
	7852, 8369, 9398, 9915, 11455, 12483, 13511, 14538, 
	16078, 17617, 18644, 20182, 21721, 23259, 24798, 26336, 
	27874, 29412, 30951, 32489, 34027, 35565, 37103, 38641, 
	40180, 41718, 43257, 44795, 46334, 47873, 49412, 50439, 
	51978, 53006, 54033, 55062, 56090, 57119, 58148, 58665, 
	59183, 59189, 59707, 59713, 59207, 59213, 58706, 57688, 
	57181, 56161, 55142, 53762, 52742, 51209, 50189, 48656, 
	47123, 46102, 44568, 43035, 41501, 39968, 38434, 36900, 
	35366, 33832, 32299, 30765, 29231, 27185, 25651, 24118, 
	23096, 21563, 20029, 18496, 16963, 15942, 14409, 13389, 
	11857, 10837, 9817, 8798, 8291, 7272, 6766, 6771, 
	6265, 6271, 6277, 6795, 7313, 7830, 8347, 9376, 
	10404, 11433, 12461, 14000, 15028, 16567, 17594, 19133, 
	20671, 22210, 23748, 25287, 26825, 28363, 29901, 31440, 
	32978, 34516, 36054, 37592, 39131, 40669, 42207, 43746, 
	45284, 46823, 48362, 49389, 50928, 51955, 53495, 54523, 
	55551, 56580, 57609, 58126, 58643, 59161, 59679, 59685, 
	59691, 59185, 58679, 58172, 57666, 56647, 56139, 55120, 
	53588, 52567, 51547, 50014, 48481, 47460, 45927, 44033, 
	42500, 40966, 39433, 37899, 36365, 34831, 33297, 31764, 
	30230, 28696, 27162, 25629, 24095, 22561, 21028, 19495, 
	17961, 16428, 15407, 13875, 12854, 11834, 10814, 9795, 
	8776, 7757, 7250, 6744, 6749, 6243, 6249, 6767, 
	6773, 7291, 7808, 8837, 9866, 10894, 11922, 12950, 
	13978, 15517, 17056, 18083, 19622, 21161, 22699, 24237, 
	25776, 27314, 28852, 30391, 31929, 33467, 35005, 36543, 
	38081, 39620, 41158, 42696, 44235, 45773, 47312, 48339, 
	49878, 51417, 52445, 53473, 55013, 56041, 56558, 57587, 
	58104, 58621, 59139, 59657, 59663, 59669, 59163, 58657, 
	58150, 57643, 56624, 55605, 54585, 53565, 52033, 51012, 
	49479, 48458, 46925, 45392, 43859, 42325, 40791, 39258, 
	37724, 36190, 34656, 33123, 31589, 30055, 28161, 26627, 
	25094, 23560, 22026, 20493, 18960, 17427, 16406, 14873, 
	13852, 12320, 11300, 10280, 9261, 8241, 7735, 7228, 
	6722, 6215, 6221, 6227, 6745, 6751, 7781, 8298, 
	8815, 9843, 10872, 11900, 13440, 14467, 16006, 17033, 
	18572, 20111, 21650, 23188, 24727, 26265, 27803, 29341, 
	30880, 32418, 33956, 35494, 37032, 38570, 40109, 41647, 
	43185, 44724, 46263, 47289, 48828, 50367, 51395, 52934, 
	53962, 54990, 56019, 57047, 57565, 58594, 59111, 59117, 
	59635, 59641, 59135, 59141, 58635, 58128, 57109, 56090, 
	55583, 54051, 53031, 52010, 50478, 49457, 47924, 46391, 
	44857, 43324, 41790, 40257, 38723, 37189, 35655, 34121, 
	32588, 31054, 29520, 27986, 26452, 24919, 23385, 21852, 
	20318, 18785, 17764, 16231, 14338, 13318, 12297, 10765, 
	9746, 9238, 8219, 7713, 7206, 6700, 6194, 6200, 
	6206, 6723, 7241, 7759, 8276, 9305, 10333, 11361, 
	12389, 13417, 14956, 15984, 17523, 19062, 20600, 21627, 
	23165, 24704, 26242, 27780, 29318, 30857, 32907, 34445, 
	35983, 37521, 39060, 40598, 42136, 43675, 45213, 46240, 
	47779, 49318, 50345, 51884, 52912, 54452, 55480, 56509, 
	57025, 58054, 58572, 59090, 59607, 59613, 59619, 59113, 
	59119, 58613, 57594, 57087, 56068, 55048, 54028, 52496, 
	51476, 49943, 48922, 47389, 45856, 44322, 42789, 41255, 
	39722, 38188, 36654, 35120, 33586, 32053, 30519, 28985, 
	27451, 25918, 24384, 22850, 21317, 19783, 18762, 17229, 
	15696, 14676, 13143, 12123, 11103, 10083, 8704, 8197, 
	7178, 6672, 6678, 6172, 6178, 6696, 6701, 7219, 
	7736, 8766, 9282, 10311, 11339, 12879, 13906, 15446, 
	16473, 18012, 19551, 20577, 22116, 23654, 25193, 26731, 
	28269, 29808, 31346, 32884, 34422, 36472, 38010, 39549, 
	41087, 42625, 43652, 45190, 46729, 48268, 49807, 50834, 
	52374, 53401, 54429, 55458, 56486, 57515, 58032, 58550, 
	59068, 59586, 59592, 59598, 59092, 58585, 58079, 57572, 
	56553, 55534, 54514, 53494, 52474, 50941, 49920, 48387, 
	46854, 45321, 44300, 42766, 41232, 39699, 38165, 36631, 
	34585, 33052, 31518, 29984, 28450, 26916, 25383, 23849, 
	22315, 20782, 19761, 18227, 16694, 15162, 14141, 13121, 
	11589, 10569, 9549, 9042, 8023, 7516, 7010, 6504, 
	6150, 6156, 6674, 6680, 7197, 8226, 8743, 9772, 
	10800, 11829, 12856, 14396, 15423, 16962, 18501, 19528, 
	21067, 22605, 24144, 25682, 27220, 28758, 30297, 31835, 
	33373, 34911, 36449, 37987, 39526, 41064, 42602, 44141, 
	45680, 47218, 48757, 49784, 51324, 52351, 53891, 54919, 
	55947, 56976, 57493, 58522, 59040, 59046, 59564, 59570, 
	59576, 59070, 58563, 58057, 57038, 56531, 55511, 54491, 
	52959, 51939, 50918, 49386, 47853, 46319, 45298, 43765, 
	42231, 40698, 39164, 37630, 36096, 34562, 33029, 30983, 
	29449, 27915, 26381, 24848, 23314, 21781, 20759, 19226

};

#define ISSData_Length 2880   // 5 days will stay below 32 Mbyte
// Remember to get ISS data:
// Lookup the UNIX epoch time: http://www.epochconverter.com/
// cd /home/thomas/Documents/hamradio/balloon/ISS/
// perl iss.pl 1335009600 5 2012-04-21_1200UTC.dat > 2012-04-21_1200UTC.tsv




#define ISS_CUT_OFF 30  // tollerance in deg (lat + lon) to consider ISS close enough to give it a try
//#define getISSLatitude(x)  ((ISSData[x] >> 9) - 64)
//#define getISSLongitude(x) ((ISSData[x] & 511) - 180)

/*
// GPS PUBX request string
#define PUBX "$PUBX,00*33"

// NMEA commands
#define GLL_OFF "$PUBX,40,GLL,0,0,0,0*5C"
#define ZDA_OFF "$PUBX,40,ZDA,0,0,0,0*44"
#define GSV_OFF "$PUBX,40,GSV,0,0,0,0*59"
#define GSA_OFF "$PUBX,40,GSA,0,0,0,0*4E"
#define RMC_OFF "$PUBX,40,RMC,0,0,0,0*47"
#define GGA_OFF "$PUBX,40,GGA,0,0,0,0*5A"
#define VTG_OFF "$PUBX,40,VTG,0,0,0,0*5E"

// configures the GPS to only use the strings we want
void configNMEA() {
  Serial.println(GLL_OFF);
  Serial.println(ZDA_OFF);
  Serial.println(GSV_OFF);
  Serial.println(GSA_OFF);
  Serial.println(RMC_OFF);
  Serial.println(GGA_OFF);
  Serial.println(VTG_OFF);

  // Set the navigation mode (Airborne, 1G)
  Serial.println("Trying to set the GPS to airborne mode:");
  unsigned int setNav[] = {
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC };
  sendUBX(setNav, sizeof(setNav)/sizeof(unsigned int));
  getUBX_ACK(setNav);
}

// Send a byte array of UBX protocol to the GPS
void sendUBX(unsigned int  *MSG, unsigned int  len) {
  for(int i=0; i<len; i++) {
    Serial.write(MSG[i]);
  }
}

// Calculate expected UBX ACK packet and parse UBX response from GPS
bool getUBX_ACK(unsigned int  *MSG) {
  uint8_t b;
  uint8_t ackByteID = 0;
  uint8_t ackPacket[10];
  int startTime = millis();

  // Construct the expected ACK packet
  ackPacket[0] = 0xB5;	// header
  ackPacket[1] = 0x62;	// header
  ackPacket[2] = 0x05;	// class
  ackPacket[3] = 0x01;	// id
  ackPacket[4] = 0x02;	// length
  ackPacket[5] = 0x00;
  ackPacket[6] = MSG[2];	// ACK class
  ackPacket[7] = MSG[3];	// ACK id
  ackPacket[8] = 0;	// CK_A
  ackPacket[9] = 0;	// CK_B

  // Calculate the checksums
  for (uint8_t i=2; i<8; i++) {
    ackPacket[8] = ackPacket[8] + ackPacket[i];
    ackPacket[9] = ackPacket[9] + ackPacket[8];
  }

  while (1) {

    // Test for success
    if (ackByteID > 9) {
      // All packets in order!
      return true;
    }

    // Timeout if no valid response in 3 seconds
    if (millis() - startTime > 3000) {
      return false;
    }

    // Make sure data is available to read
    if (Serial.available()) {
      b = Serial.read();

      // Check that bytes arrive in sequence as per expected ACK packet
      if (b == ackPacket[ackByteID]) {
        ackByteID++;
      }
      else {
        ackByteID = 0;	// Reset and look again, invalid order
      }

    }
  }
}

*/

// Module declarations
static void parse_sentence_type(const char * token);
static void parse_time(const char *token);
static void parse_date(const char *token);
static void parse_status(const char *token);
static void parse_lat(const char *token);
static void parse_lat_hemi(const char *token);
static void parse_lon(const char *token);
static void parse_lon_hemi(const char *token);
static void parse_speed(const char *token);
static void parse_course(const char *token);
static void parse_altitude(const char *token);



// Module types
typedef void (*t_nmea_parser)(const char *token);

enum t_sentence_type {
  SENTENCE_UNK,
  SENTENCE_GGA,
  SENTENCE_RMC
};


// Module constants
static const t_nmea_parser unk_parsers[] = {
  parse_sentence_type,    // $GPxxx
};

static const t_nmea_parser gga_parsers[] = {
  NULL,             // $GPGGA
  parse_time,       // Time
  NULL,             // Latitude
  NULL,             // N/S
  NULL,             // Longitude
  NULL,             // E/W
  NULL,             // Fix quality 
  NULL,             // Number of satellites
  NULL,             // Horizontal dilution of position
  parse_altitude,   // Altitude
  NULL,             // "M" (mean sea level)
  NULL,             // Height of GEOID (MSL) above WGS84 ellipsoid
  NULL,             // "M" (mean sea level)
  NULL,             // Time in seconds since the last DGPS update
  NULL              // DGPS station ID number
};

static const t_nmea_parser rmc_parsers[] = {
  NULL,             // $GPRMC
  parse_time,       // Time
  parse_status,     // A=active, V=void
  parse_lat,        // Latitude,
  parse_lat_hemi,   // N/S
  parse_lon,        // Longitude
  parse_lon_hemi,   // E/W
  parse_speed,      // Speed over ground in knots
  parse_course,     // Track angle in degrees (true)
  parse_date,       // Date (DDMMYY)
  NULL,             // Magnetic variation
  NULL              // E/W
};

static const int NUM_OF_UNK_PARSERS = (sizeof(unk_parsers) / sizeof(t_nmea_parser));
static const int NUM_OF_GGA_PARSERS = (sizeof(gga_parsers) / sizeof(t_nmea_parser));
static const int NUM_OF_RMC_PARSERS = (sizeof(rmc_parsers) / sizeof(t_nmea_parser));

// Module variables
static t_sentence_type sentence_type = SENTENCE_UNK;
static bool at_checksum = false;
static unsigned char our_checksum = '$';
static unsigned char their_checksum = 0;
static char token[16];
static int num_tokens = 0;
static unsigned int offset = 0;
static bool active = false;
static char gga_time[7], rmc_time[7], rmc_date[7];
static char new_time[7], new_date[7];
static float new_lat;
static float new_lon;
static char new_aprs_lat[9];
static char new_aprs_lon[10];
static float new_course;
static float new_speed;
static float new_altitude;

// Public (extern) variables, readable from other modules
char gps_time[7];       // HHMMSS
char gps_date[7];       // DDMMYY
float gps_lat = 0;
float gps_lon = 0;
char gps_aprs_lat[9];
char gps_aprs_lon[10];
float gps_course = 0;
float gps_speed = 0;
float gps_altitude = -1000.0; //The dead sea at -420 m is theoretically the deepest usable spot on earth where you could use a GPS
                              //Here we define -1000 m as our invalid gps altitude 
unsigned long doppler_frequency = 145825000UL;
bool satInView = false;
int iss_lat;
int iss_lon;
unsigned int iss_datapoint;

// Module functions
unsigned char from_hex(char a) 
{
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else if (a >= '0' && a <= '9')
    return a - '0';
  else
    return 0;
}

void parse_sentence_type(const char *token)
{
  if (strcmp(token, "$GPGGA") == 0) {
    sentence_type = SENTENCE_GGA;
  } else if (strcmp(token, "$GPRMC") == 0) {
    sentence_type = SENTENCE_RMC;
  } else {
    sentence_type = SENTENCE_UNK;
  }
}

void parse_time(const char *token)
{
  // Time can have decimals (fractions of a second), but we only take HHMMSS
  strncpy(new_time, token, 6);
}

void parse_date(const char *token)
{
  // Date in DDMMYY
  strncpy(new_date, token, 6);
}


void parse_status(const char *token)
{
  // "A" = active, "V" = void. We shoud disregard void sentences
  if (strcmp(token, "A") == 0)
    active = true;
  else
    active = false;
}

void parse_lat(const char *token)
{
  // Parses latitude in the format "DD" + "MM" (+ ".M{...}M")
  char degs[3];
  if (strlen(token) >= 4) {
    degs[0] = token[0];
    degs[1] = token[1];
    degs[2] = '\0';
    new_lat = atof(degs) + atof(token + 2) / 60;
  }
  // APRS-ready latitude
  strncpy(new_aprs_lat, token, 7);
}

void parse_lat_hemi(const char *token)
{
  if (token[0] == 'S')
    new_lat = -new_lat;
  new_aprs_lat[7] = token[0];
  new_aprs_lon[8] = '\0';
}

void parse_lon(const char *token)
{
  // Longitude is in the format "DDD" + "MM" (+ ".M{...}M")
  char degs[4];
  if (strlen(token) >= 5) {
    degs[0] = token[0];
    degs[1] = token[1];
    degs[2] = token[2];
    degs[3] = '\0';
    new_lon = atof(degs) + atof(token + 3) / 60;
  }
  // APRS-ready longitude
  strncpy(new_aprs_lon, token, 8);
}

void parse_lon_hemi(const char *token)
{
  if (token[0] == 'W')
    new_lon = -new_lon;
  new_aprs_lon[8] = token[0];
  new_aprs_lon[9] = '\0';
}

void parse_speed(const char *token)
{
  new_speed = atof(token);
}

void parse_course(const char *token)
{
  new_course = atof(token);
}

void parse_altitude(const char *token)
{
  new_altitude = atof(token);
}


//
// Exported functions
//
void gps_setup() {
  strcpy(gps_time, "000000");
  strcpy(gps_date, "000000");
  strcpy(gps_aprs_lat, "0000.00N");
  strcpy(gps_aprs_lon, "00000.00E");
  
  
  // Set uBlox navigation mode
  for(int i=0; i<44; i++) {
    Serial.write(pgm_read_byte_near(ubx_setNav + i ));
    
  }
  
  
  
}

bool gps_decode(char c)
{
  int ret = false;

  switch(c) {
    case '\r':
    case '\n':
      // End of sentence

      if (num_tokens && our_checksum == their_checksum) {
#ifdef DEBUG_GPS
        Serial.print(" (OK!)");
#endif
        // Return a valid position only when we've got two rmc and gga
        // messages with the same timestamp.
        switch (sentence_type) {
          case SENTENCE_UNK:
            break;    // Keeps gcc happy
          case SENTENCE_GGA:
            strcpy(gga_time, new_time);
            break;
          case SENTENCE_RMC:
            strcpy(rmc_time, new_time);
            strcpy(rmc_date, new_date);
            break;
        }

        // Valid position scenario:
        //
        // 1. The timestamps of the two previous GGA/RMC sentences must match.
        //
        // 2. We just processed a known (GGA/RMC) sentence. Suppose the
        //    contrary: after starting up this module, gga_time and rmc_time
        //    are both equal (they're both initialized to ""), so (1) holds
        //    and we wrongly report a valid position.
        //
        // 3. The GPS has a valid fix. For some reason, the Venus 634FLPX
        //    reports 24 deg N, 121 deg E (the middle of Taiwan) until a valid
        //    fix is acquired:
        //
        //    $GPGGA,120003.000,2400.0000,N,12100.0000,E,0,00,0.0,0.0,M,0.0,M,,0000**69 (OK!)
        //    $GPGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0**30 (OK!)
        //    $GPRMC,120003.000,V,2400.0000,N,12100.0000,E,000.0,000.0,280606,,,N**78 (OK!)
        //    $GPVTG,000.0,T,,M,000.0,N,000.0,K,N**02 (OK!)
        //
        // 4.) We also want a clean altitude being reported.
        //     When is an altitude clean? Theoretically we could launch a balloon from the dead sea
        //     420 m below sea level. Therefore we define -1000 m as our invalid altitude and
        //     expect to find an altitude above -420 m when we've decoded a valid GGA statement.

        if (sentence_type != SENTENCE_UNK &&      // Known sentence?
            strcmp(gga_time, rmc_time) == 0 &&    // RMC/GGA times match?
            active &&                             // Valid fix?
            new_altitude > -1000.0) {             // Valid new altitude?              
          // Atomically merge data from the two sentences
          strcpy(gps_time, new_time);
          strcpy(gps_date, new_date);
          gps_lat = new_lat;
          gps_lon = new_lon;
          strcpy(gps_aprs_lat, new_aprs_lat);
          strcpy(gps_aprs_lon, new_aprs_lon);
          gps_course = new_course;
          gps_speed = new_speed;
          gps_altitude = new_altitude;
          new_altitude = -1000.0; // Invalidate new_altitude so that we are sure to get a valid one next time
          ret = true;
        }
      }
#ifdef DEBUG_GPS
      if (num_tokens)
        Serial.println();
#endif
      at_checksum = false;        // CR/LF signals the end of the checksum
      our_checksum = '$';         // Reset checksums
      their_checksum = 0;
      offset = 0;                 // Prepare for the next incoming sentence
      num_tokens = 0;
      sentence_type = SENTENCE_UNK;
      break;
    
    case '*':
      // Begin of checksum and process token (ie. do not break)
      at_checksum = true;
      our_checksum ^= c;
#ifdef DEBUG_GPS
      Serial.print(c);
#endif

    case ',':
      // Process token
      token[offset] = '\0';
      our_checksum ^= c;  // Checksum the ',', undo the '*'

      // Parse token
      switch (sentence_type) {
        case SENTENCE_UNK:
          if (num_tokens < NUM_OF_UNK_PARSERS && unk_parsers[num_tokens])
            unk_parsers[num_tokens](token);
          break;
        case SENTENCE_GGA:
          if (num_tokens < NUM_OF_GGA_PARSERS && gga_parsers[num_tokens])
            gga_parsers[num_tokens](token);
          break;
        case SENTENCE_RMC:
          if (num_tokens < NUM_OF_RMC_PARSERS && rmc_parsers[num_tokens])
            rmc_parsers[num_tokens](token);
          break;
      }

      // Prepare for next token
      num_tokens++;
      offset = 0;
#ifdef DEBUG_GPS
      Serial.print(c);
#endif
      break;

    default:
      // Any other character
      if (at_checksum) {
        // Checksum value
        their_checksum = their_checksum * 16 + from_hex(c);
      } else {
        // Regular NMEA data
        if (offset < 15) {  // Avoid buffer overrun (tokens can't be > 15 chars)
          token[offset] = c;
          offset++;
          our_checksum ^= c;
        }
      }
#ifdef DEBUG_GPS
      Serial.print(c);
#endif
  }
  return ret;
}


unsigned long gps_get_region_frequency()
{
  unsigned long frequency = RADIO_FREQUENCY;
  if(-168 < gps_lon && gps_lon < -25) frequency = RADIO_FREQUENCY_REGION2;
  if(-25 <  gps_lon && gps_lon <  71) frequency = RADIO_FREQUENCY_REGION1;
  if(-34.95 < gps_lat && gps_lat < 7.18  && -73.13 < gps_lon && gps_lon < -26.46) frequency = RADIO_FREQUENCY_BRAZIL;      // Brazil
  if( 29.38 < gps_lat && gps_lat < 47.10 && 127.16 < gps_lon && gps_lon < 153.61) frequency = RADIO_FREQUENCY_JAPAN;       // Japan
  if( 19.06 < gps_lat && gps_lat < 53.74 &&  72.05 < gps_lon && gps_lon < 127.16) frequency = RADIO_FREQUENCY_CHINA;       // China
  if( -0.30 < gps_lat && gps_lat < 20.42 &&  93.06 < gps_lon && gps_lon < 105.15) frequency = RADIO_FREQUENCY_THAILAND;    // Thailand
  if(-54.54 < gps_lat && gps_lat <-32.43 && 161.62 < gps_lon && gps_lon < 179.99) frequency = RADIO_FREQUENCY_NEWZEALAND;  // New Zealand
  if(-50.17 < gps_lat && gps_lat < -8.66 && 105.80 < gps_lon && gps_lon < 161.62) frequency = RADIO_FREQUENCY_AUSTRALIA;   // Australia
  
  // Note: These regions are a super simplified approach to define rectangles on the world map, representing regions where we may consider at least some
  // chance that an APRS Digipeter or an Igate is listening. They have absolutely NO political relevance. I was just trying to identify
  // regions with APRS activity on APRS.fi and look up the used frequencies on the web. Any corrections or additions are welcome. Please email
  // your suggestions to thomas@tkrahn.com 
  
  // use your own coordinates for testing; Comment out when testing is finished!
//  if(29.7252 < gps_lat && gps_lat < 29.7261 && -95.5082 < gps_lon && gps_lon < -95.5074) frequency = MX146_FREQUENCY_TESTING; // KT5TK home
  //if(29.7353 < gps_lat && gps_lat < 29.7359 && -95.5397 < gps_lon && gps_lon < -95.5392) frequency = MX146_FREQUENCY_TESTING; // Gessner BBQ

  // Note: Never define a region that spans the date line! Use two regions instead.
  if(gps_lat == 0 && gps_lon == 0) frequency = RADIO_FREQUENCY; // switch to default when we don't have a GPS lease
  
  return frequency;
}

bool gps_check_satellite()
{
  int latdiff;
  int londiff;

  long igpsdate = atol(gps_date);
  long igpstime = atol(gps_time);
  
  int gpstime_day     =        igpsdate / 10000;  
  int gpstime_month   =        (igpsdate % 10000) / 100;
  int gpstime_year    =        (igpsdate % 10000) % 100 + 2000;
  int gpstime_hour    =        igpstime / 10000;
  int gpstime_minute  =        (igpstime % 10000) / 100; 
  int gpstime_second  =        (igpstime % 10000) % 100;

  
//  int gpstime_year    = 2012;  
//  int gpstime_month   = 2;
//  int gpstime_day     = 11;
//  int gpstime_hour    = 21;
//  int gpstime_minute  = 0;
//  int gpstime_second  = 0;


// The start time must match with the dataset above!
  long gpslaunchminutes = (gpstime_day - 8 ) * 1440 + (gpstime_hour - 23) * 60 + gpstime_minute - 0;



// look 2 minutes into the future so that you see the constellation for the next TX cycle
  gpslaunchminutes += 2;
  
  // make sure we're in the bounds of the array.
  if ((gpslaunchminutes < 0) || (gpslaunchminutes > ISSData_Length)) // make sure we're in the bounds of the array.
  {
    gpslaunchminutes = 0; 
  }
  
  iss_datapoint = pgm_read_word_near(ISSData + gpslaunchminutes);
  
  // unmerge the datapoint into its components
  
  iss_lat = ((iss_datapoint >> 9) - 64); 
  iss_lon = ((iss_datapoint & 511) - 180);
  
  latdiff = abs(iss_lat - (int)gps_lat);
  londiff = abs(iss_lon - (int)gps_lon);
  
  // ISS nearby? This doesn't really represent a circle (rather a rhombus or something, but who cares?
  if ((latdiff + londiff) < ISS_CUT_OFF) 
  {
    satInView = true;
    
  }
  else
  {
    satInView = false;

  }  
  return satInView;  
}





float gps_get_lat()
{
  return gps_lat; 
}


float gps_get_lon()
{
  return gps_lon; 
}

float gps_get_altitude()
{
  return gps_altitude; 
}


long gps_get_time()
{
  return atol(gps_time);
}

long gps_get_date()
{
  return atol(gps_date);
}





