import csv
import matplotlib.pyplot as plt
import numpy as np

# -----------------------------------
# 				DATA
# -----------------------------------

serverTime = [749724709, 759733384, 769742054, 779750725, 789759399, 799768076, 809776747, 819785415, 829794083, 839802750, 849811423, 859820096, 869828764, 879837439, 889846104, 899854768, 909863439, 919872105, 929880776, 939889442, 949898117, 959906790, 969915456, 979924126, 989932795, 999941469, 1009950135, 1019958803, 1029967473, 1039976139, 1049984811, 1059993485, 1070002157, 1080010825, 1090019489, 1100028159, 1110036829, 1120045500, 1130054172, 1140062836, 1150071505, 1160080174, 1170088843, 1180097511, 1190106181, 1200114847, 1210123513, 1220132181, 1230140849, 1240149513, 1250158185, 1260166851, 1270175520, 1280184195, 1290192864, 1300201534, 1310210208, 1320218882, 1330227551, 1340236223, 1350244888, 1360253559, 1370262226, 1380270899, 1390279566, 1400288236, 1410296899, 1420305568, 1430314237, 1440322901, 1450331565, 1460340228, 1470348895, 1480357567, 1490366238, 1500374923, 1510383602, 1520392288, 1530400973, 1540409647, 1550418321, 1560427008, 1570435697, 1580444379, 1590453066, 1600461748, 1610470433, 1620479121, 1630487806, 1640496485, 1650505167, 1660513857, 1670522545, 1680531236, 1690539912, 1700548581, 1710557250, 1720565927, 1730574599, 1740583274, 1750591950, 1760600622, 1770609310, 1780617994, 1790626673, 1800635350, 1810644026, 1820652697, 1830661366, 1840670036, 1850678711, 1860687384, 1870696058, 1880704736, 1890713411, 1900722091, 1910730763, 1920739441, 1930748110, 1940756779, 1950765450, 1960774125, 1970782799, 1980791473, 1990800151, 2000808827, 2010817503, 2020826185, 2030834861, 2040843538, 2050852219, 2060860897, 2070869579, 2080878251, 2090886927, 2100895606, 2110904285, 2120912980, 2130921665, 2140930344, 2150939021, 2160947694, 2170956371, 2180965047, 2190973717, 2200982387, 2210991063, 2220999739, 2231008412, 2241017080, 2251025748, 2261034425, 2271043099, 2281051768, 2291060442, 2301069114, 2311077792, 2321086465, 2331095134, 2341103811, 2351112484, 2361121160, 2371129834, 2381138507, 2391147184, 2401155861, 2411164535, 2421173211, 2431181880, 2441190554, 2451199224, 2461207898, 2471216571, 2481225242, 2491233914, 2501242590, 2511251266, 2521259932, 2531268606, 2541277278, 2551285946, 2561294616, 2571303285, 2581311958, 2591320633, 2601329314, 2611337982, 2621346649, 2631355317, 2641363986, 2651372656, 2661381326, 2671389994, 2681398667]
t0A = [624057378, 634066131, 644074879, 654083628, 664092381, 674101136, 684109885, 694118631, 704127377, 714136123, 724144875, 734153625, 744162372, 754171125, 764179868, 774188611, 784197359, 794206105, 804214854, 814223598, 824232351, 834241103, 844249847, 854258595, 864267343, 874276095, 884284839, 894293585, 904302334, 914311079, 924319829, 934328581, 944337331, 954346077, 964354820, 974363569, 984372317, 994381067, 1004389817, 1014398560, 1024407306, 1034416054, 1044424802, 1054433548, 1064442297, 1074451042, 1084459785, 1094468532, 1104477279, 1114486021, 1124494771, 1134503517, 1144512263, 1154521017, 1164529764, 1174538512, 1184547265, 1194556018, 1204564765, 1214573516, 1224582260, 1234591009, 1244599755, 1254608506, 1264617252, 1274626000, 1284634742, 1294643489, 1304652237, 1314660979, 1324669721, 1334678463, 1344687208, 1354695959, 1364704709, 1374713472, 1384722230, 1394730994, 1404739757, 1414748510, 1424757263, 1434766029, 1444774796, 1454783557, 1464792323, 1474801083, 1484809847, 1494818613, 1504827377, 1514836134, 1524844895, 1534853663, 1544862430, 1554871199, 1564879953, 1574888701, 1584897448, 1594906204, 1604914954, 1614923707, 1624932461, 1634941212, 1644949978, 1654958740, 1664967498, 1674976253, 1684985007, 1694993757, 1705002504, 1715011252, 1725020005, 1735028757, 1745037509, 1755046264, 1765055017, 1775063776, 1785072526, 1795081283, 1805090029, 1815098777, 1825107526, 1835116279, 1845125031, 1855133783, 1865142540, 1875151295, 1885160048, 1895168808, 1905177563, 1915186319, 1925195077, 1935203835, 1945212595, 1955221345, 1965230099, 1975238856, 1985247614, 1995256388, 2005265151, 2015273908, 2025282663, 2035291414, 2045300170, 2055308924, 2065317673, 2075326420, 2085335174, 2095343929, 2105352680, 2115361427, 2125370173, 2135378928, 2145387680, 2155396427, 2165405180, 2175413930, 2185422686, 2195431437, 2205440185, 2215448940, 2225457691, 2235466445, 2245475198, 2255483950, 2265492705, 2275501460, 2285510213, 2295518967, 2305527714, 2315536467, 2325545216, 2335553968, 2345562720, 2355571469, 2365580220, 2375588974, 2385597729, 2395606473, 2405615226, 2415623976, 2425632722, 2435641470, 2445650218, 2455658970, 2465667723, 2475676482, 2485685229, 2495693974, 2505702720, 2515711468, 2525720216, 2535728965, 2545737711, 2555746462]
estimationA = [749726622, 759735339, 769744050, 779752763, 789761479, 799770198, 809778910, 819787064, 829795765, 839804465, 849813172, 859821876, 869830577, 879838988, 889847681, 899856375, 909865073, 919873769, 929882468, 939890982, 949899683, 959908383, 969917075, 979925771, 989934467, 999943267, 1009951960, 1019960656, 1029969355, 1039978050, 1049986750, 1059995519, 1070004220, 1080012917, 1090021611, 1100030311, 1110039010, 1120047296, 1130055992, 1140064681, 1150073373, 1160082068, 1170090762, 1180099142, 1190107834, 1200116522, 1210125207, 1220133897, 1230142587, 1240151124, 1250159816, 1260168503, 1270177191, 1280185886, 1290194575, 1300203166, 1310211859, 1320220553, 1330229241, 1340237932, 1350246617, 1360255306, 1370263554, 1380272241, 1390280924, 1400289608, 1410298286, 1420306969, 1430315724, 1440324403, 1450333082, 1460341761, 1470350443, 1480359131, 1490367576, 1500376273, 1510384966, 1520393665, 1530402363, 1540411051, 1550419411, 1560428109, 1570436808, 1580445501, 1590454199, 1600462891, 1610471587, 1620480049, 1630488743, 1640497430, 1650506121, 1660514819, 1670523516, 1680532061, 1690540743, 1700549420, 1710558096, 1720566781, 1730575460, 1740583898, 1750592579, 1760601257, 1770609950, 1780618638, 1790627323, 1800635743, 1810644422, 1820653097, 1830661769, 1840670442, 1850679120, 1860687609, 1870696284, 1880704963, 1890713639, 1900722322, 1910730995, 1920739676, 1930748170, 1940756840, 1950765511, 1960774187, 1970782861, 1980791535, 1990800188, 2000808865, 2010817540, 2020826222, 2030834899, 2040843578, 2050852282, 2060860963, 2070869645, 2080878317, 2090886994, 2100895673, 2110904525, 2120913222, 2130921909, 2140930589, 2150939268, 2160947942, 2170956741, 2180965419, 2190974092, 2200982763, 2210991442, 2221000121, 2231008815, 2241017486, 2251026157, 2261034836, 2271043513, 2281052184, 2291061020, 2301069695, 2311078377, 2321087053, 2331095727, 2341104408, 2351113022, 2361121701, 2371130379, 2381139056, 2391147737, 2401156417, 2411165065, 2421173744, 2431182416, 2441191095, 2451199769, 2461208446, 2471217175, 2481225850, 2491234526, 2501243206, 2511251887, 2521260557, 2531269236, 2541277958, 2551286631, 2561295305, 2571303980, 2581312659, 2591321338, 2601330091, 2611338766, 2621347438, 2631356111, 2641364786, 2651373462, 2661382235, 2671390909, 2681399588]

# -----------------------------------
# 			Node 1 Plot
# -----------------------------------

plt.figure('Phi', figsize=(10,10)) 
plt.title('Node 1')


plt.plot(serverTime, [b - a for a,b in zip(serverTime, t0A)], "k-", label="Real Phi Node 1")
plt.plot(serverTime, [b - a for a,b in zip(estimationA, t0A)], "r-", label="Estimated Phi Node 1")

plt.xlabel('ServerTime (microsends)')
plt.ylabel('Phi (microsends)')

axes = plt.gca()

plt.legend()
plt.savefig('oneClient-Node1.png')
plt.close()

# -----------------------------------
# 			Phi Error Plot
# -----------------------------------

plt.figure('Phi', figsize=(10,10)) 
plt.title('Phi Error')

error1 = [abs(b - a) for a,b in zip(estimationA, serverTime)]

plt.plot(serverTime, error1, "r-", label="Node 1")

plt.xlabel('ServerTime (microsends)')
plt.ylabel('Phi Error (microsends)')

plt.legend()
plt.savefig('oneClient-Error.png')
plt.close()

print("Node1:") 
print("\tMin Error: " + str(min(error1)))
print("\tMax Error: " + str(max(error1)))
