// Molecule Library - XYZ coordinates for common molecules
const MOLECULE_LIBRARY = {
    water: {
        name: "Water",
        formula: "H₂O",
        xyz: `3
Water molecule
O    0.000000    0.000000    0.119262
H    0.000000    0.757570   -0.477047
H    0.000000   -0.757570   -0.477047`
    },
    methane: {
        name: "Methane", 
        formula: "CH₄",
        xyz: `5
Methane molecule
C    0.000000    0.000000    0.000000
H    0.629118    0.629118    0.629118
H   -0.629118   -0.629118    0.629118
H   -0.629118    0.629118   -0.629118
H    0.629118   -0.629118   -0.629118`
    },
    ammonia: {
        name: "Ammonia",
        formula: "NH₃", 
        xyz: `4
Ammonia molecule
N    0.000000    0.000000    0.112985
H    0.000000    0.937100   -0.263631
H    0.811441   -0.468550   -0.263631
H   -0.811441   -0.468550   -0.263631`
    },
    carbon_dioxide: {
        name: "Carbon Dioxide",
        formula: "CO₂",
        xyz: `3
Carbon dioxide molecule
C    0.000000    0.000000    0.000000
O    0.000000    0.000000    1.162323
O    0.000000    0.000000   -1.162323`
    },
    ethanol: {
        name: "Ethanol",
        formula: "C₂H₆O",
        xyz: `9
Ethanol molecule
C   -0.748380   -0.015520    0.024370
C    0.716990    0.026040   -0.061040
O    1.165720    1.232210    0.445470
H   -1.166000   -0.563000   -0.818200
H   -1.115510   -0.449650    0.962500
H   -1.115510    1.017350    0.024370
H    1.115510   -0.808200   -0.563000
H    1.115510    0.026040   -1.115510
H    2.115510    1.232210    0.445470`
    },
    acetone: {
        name: "Acetone",
        formula: "C₃H₆O",
        xyz: `10
Acetone molecule
C    0.000000    0.000000    0.000000
C    1.507000    0.000000    0.000000
C   -0.753500   -1.305000    0.000000
O   -0.753500    1.305000    0.000000
H    1.880500   -0.522500   -0.904500
H    1.880500   -0.522500    0.904500
H    1.880500    1.045000    0.000000
H   -0.427000   -1.827500   -0.904500
H   -0.427000   -1.827500    0.904500
H   -1.834500   -1.305000    0.000000`
    },
    benzene: {
        name: "Benzene",
        formula: "C₆H₆",
        xyz: `12
Benzene molecule
C    1.396000    0.000000    0.000000
C    0.698000    1.209000    0.000000
C   -0.698000    1.209000    0.000000
C   -1.396000    0.000000    0.000000
C   -0.698000   -1.209000    0.000000
C    0.698000   -1.209000    0.000000
H    2.485000    0.000000    0.000000
H    1.242500    2.152000    0.000000
H   -1.242500    2.152000    0.000000
H   -2.485000    0.000000    0.000000
H   -1.242500   -2.152000    0.000000
H    1.242500   -2.152000    0.000000`
    },
    toluene: {
        name: "Toluene",
        formula: "C₇H₈",
        xyz: `15
Toluene molecule
C    0.000000    0.000000    0.000000
C    1.396000    0.000000    0.000000
C    2.094000    1.209000    0.000000
C    1.396000    2.418000    0.000000
C    0.000000    2.418000    0.000000
C   -0.698000    1.209000    0.000000
C   -0.698000   -1.396000    0.000000
H    1.943000   -0.943000    0.000000
H    3.183000    1.209000    0.000000
H    1.943000    3.361000    0.000000
H   -0.547000    3.361000    0.000000
H   -1.787000    1.209000    0.000000
H   -0.174000   -2.339000    0.000000
H   -1.787000   -1.396000    0.000000
H   -0.174000   -1.396000    0.943000`
    },
    acetonitrile: {
        name: "Acetonitrile",
        formula: "C₂H₃N",
        xyz: `6
Acetonitrile molecule
C    0.000000    0.000000    0.000000
C    0.000000    0.000000    1.458000
N    0.000000    0.000000    2.620000
H    1.028000    0.000000   -0.381000
H   -0.514000   -0.890000   -0.381000
H   -0.514000    0.890000   -0.381000`
    },
    caffeine: {
        name: "Caffeine",
        formula: "C₈H₁₀N₄O₂",
        xyz: `24
Caffeine molecule
N   -1.274000   -0.226000    0.000000
C   -0.890000    1.067000    0.000000
N    0.408000    1.067000    0.000000
C    0.792000   -0.226000    0.000000
C   -0.274000   -1.067000    0.000000
C    2.158000   -0.226000    0.000000
O    2.792000   -1.274000    0.000000
N    2.792000    0.890000    0.000000
C    2.158000    2.158000    0.000000
N    0.890000    2.158000    0.000000
C   -2.658000   -0.658000    0.000000
C    4.226000    0.890000    0.000000
O   -0.658000   -2.274000    0.000000
H   -1.524000    1.931000    0.000000
H    2.792000    3.022000    0.000000
H    0.408000    3.022000    0.000000
H   -3.022000   -0.274000    0.890000
H   -3.022000   -0.274000   -0.890000
H   -2.658000   -1.750000    0.000000
H    4.590000    0.408000    0.890000
H    4.590000    0.408000   -0.890000
H    4.590000    1.931000    0.000000`
    },
    aspirin: {
        name: "Aspirin",
        formula: "C₉H₈O₄",
        xyz: `21
Aspirin molecule
C    0.000000    0.000000    0.000000
C    1.396000    0.000000    0.000000
C    2.094000    1.209000    0.000000
C    1.396000    2.418000    0.000000
C    0.000000    2.418000    0.000000
C   -0.698000    1.209000    0.000000
C   -0.698000   -1.396000    0.000000
O   -2.094000   -1.396000    0.000000
C   -2.792000   -2.605000    0.000000
O   -4.018000   -2.605000    0.000000
C    3.490000    1.209000    0.000000
O    3.490000    2.605000    0.000000
O    4.716000    0.513000    0.000000
H    1.943000   -0.943000    0.000000
H    1.943000    3.361000    0.000000
H   -0.547000    3.361000    0.000000
H   -1.787000    1.209000    0.000000
H   -0.174000   -2.339000    0.000000
H   -2.268000   -3.548000    0.000000
H    4.433000    2.605000    0.000000`
    },
    glucose: {
        name: "Glucose",
        formula: "C₆H₁₂O₆",
        xyz: `24
Glucose molecule
C    0.000000    0.000000    0.000000
C    1.396000    0.000000    0.000000
C    2.094000    1.209000    0.000000
C    1.396000    2.418000    0.000000
C    0.000000    2.418000    0.000000
O   -0.698000    1.209000    0.000000
O   -0.698000   -1.209000    0.000000
O    2.094000   -1.209000    0.000000
O    3.490000    1.209000    0.000000
O    2.094000    3.627000    0.000000
O   -0.698000    3.627000    0.000000
H    0.000000    0.000000    1.089000
H    1.396000    0.000000    1.089000
H    2.094000    1.209000    1.089000
H    1.396000    2.418000    1.089000
H    0.000000    2.418000    1.089000
H   -1.658000   -1.209000    0.000000
H    3.054000   -1.209000    0.000000
H    4.450000    1.209000    0.000000
H    3.054000    3.627000    0.000000
H   -1.658000    3.627000    0.000000`
    },
    chloroform: {
        name: "Chloroform",
        formula: "CHCl₃",
        xyz: `5
Chloroform molecule
C    0.000000    0.000000    0.000000
Cl   0.000000    0.000000    1.776000
Cl   1.676000   -0.968000   -0.592000
Cl  -1.676000   -0.968000   -0.592000
H    0.000000    1.936000   -0.592000`
    },
    dmso: {
        name: "DMSO",
        formula: "C₂H₆OS",
        xyz: `10
DMSO molecule
S    0.000000    0.000000    0.000000
O    0.000000    0.000000    1.504000
C    1.813000    0.000000   -0.752000
C   -1.813000    0.000000   -0.752000
H    1.813000    0.890000   -1.376000
H    1.813000   -0.890000   -1.376000
H    2.703000    0.000000   -0.128000
H   -1.813000   -0.890000   -1.376000
H   -1.813000    0.890000   -1.376000
H   -2.703000    0.000000   -0.128000`
    },
    thf: {
        name: "THF",
        formula: "C₄H₈O",
        xyz: `13
THF molecule
O    0.000000    0.000000    0.000000
C    1.396000    0.000000    0.000000
C    2.094000    1.396000    0.000000
C    1.396000    2.094000    1.396000
C    0.000000    1.396000    1.396000
H    1.396000    0.000000   -1.089000
H    1.396000   -1.089000    0.000000
H    3.183000    1.396000    0.000000
H    2.094000    1.396000   -1.089000
H    1.396000    3.183000    1.396000
H    1.396000    2.094000    2.485000
H   -1.089000    1.396000    1.396000
H    0.000000    1.396000    2.485000`
    },
    ethyl_acetate: {
        name: "Ethyl Acetate",
        formula: "C₄H₈O₂",
        xyz: `14
Ethyl acetate molecule
C    0.000000    0.000000    0.000000
C    1.507000    0.000000    0.000000
O    2.205000    1.162000    0.000000
O    2.205000   -1.162000    0.000000
C    3.712000   -1.162000    0.000000
C    4.410000   -2.324000    0.000000
H   -0.373000   -0.522000   -0.904500
H   -0.373000   -0.522000    0.904500
H   -0.373000    1.045000    0.000000
H    4.085000   -0.640000   -0.904500
H    4.085000   -0.640000    0.904500
H    4.037000   -2.846000   -0.904500
H    4.037000   -2.846000    0.904500
H    5.499000   -2.324000    0.000000`
    },
    dichloromethane: {
        name: "Dichloromethane",
        formula: "CH₂Cl₂",
        xyz: `5
Dichloromethane molecule
C    0.000000    0.000000    0.000000
Cl   0.000000    0.000000    1.776000
Cl   0.000000    1.776000    0.000000
H    1.089000   -0.363000   -0.363000
H   -1.089000   -0.363000   -0.363000`
    },
    hexane: {
        name: "Hexane",
        formula: "C₆H₁₄",
        xyz: `20
Hexane molecule
C    0.000000    0.000000    0.000000
C    1.507000    0.000000    0.000000
C    2.205000    1.396000    0.000000
C    3.712000    1.396000    0.000000
C    4.410000    2.792000    0.000000
C    5.917000    2.792000    0.000000
H   -0.373000   -0.522000   -0.904500
H   -0.373000   -0.522000    0.904500
H   -0.373000    1.045000    0.000000
H    1.880000   -0.522000   -0.904500
H    1.880000   -0.522000    0.904500
H    1.832000    1.918000   -0.904500
H    1.832000    1.918000    0.904500
H    4.085000    0.874000   -0.904500
H    4.085000    0.874000    0.904500
H    4.037000    3.314000   -0.904500
H    4.037000    3.314000    0.904500
H    6.290000    2.270000   -0.904500
H    6.290000    2.270000    0.904500
H    6.290000    3.837000    0.000000`
    },
    glycine: {
        name: "Glycine",
        formula: "C₂H₅NO₂",
        xyz: `10
Glycine molecule
C    0.000000    0.000000    0.000000
C    1.507000    0.000000    0.000000
O    2.205000    1.162000    0.000000
O    2.205000   -1.162000    0.000000
N   -0.698000    1.396000    0.000000
H   -0.373000   -0.522000   -0.904500
H   -0.373000   -0.522000    0.904500
H   -0.325000    1.918000   -0.827000
H   -0.325000    1.918000    0.827000
H    3.165000   -1.162000    0.000000`
    },
    alanine: {
        name: "Alanine",
        formula: "C₃H₇NO₂",
        xyz: `13
Alanine molecule
C    0.000000    0.000000    0.000000
C    1.507000    0.000000    0.000000
C   -0.698000    1.396000    0.000000
O    2.205000    1.162000    0.000000
O    2.205000   -1.162000    0.000000
N   -0.698000   -1.396000    0.000000
H   -0.373000   -0.522000   -0.904500
H   -0.325000    1.918000   -0.904500
H   -0.325000    1.918000    0.904500
H   -1.787000    1.396000    0.000000
H   -0.325000   -1.918000   -0.827000
H   -0.325000   -1.918000    0.827000
H    3.165000   -1.162000    0.000000`
    },
    hydrogen_peroxide: {
        name: "Hydrogen Peroxide",
        formula: "H₂O₂",
        xyz: `4
Hydrogen peroxide molecule
O    0.000000    0.000000    0.000000
O    1.452000    0.000000    0.000000
H   -0.363000    0.890000    0.000000
H    1.815000   -0.890000    0.000000`
    },
    sulfur_dioxide: {
        name: "Sulfur Dioxide",
        formula: "SO₂",
        xyz: `3
Sulfur dioxide molecule
S    0.000000    0.000000    0.000000
O    0.000000    0.000000    1.481000
O    0.000000    1.481000   -0.741000`
    },
    nitric_oxide: {
        name: "Nitric Oxide",
        formula: "NO",
        xyz: `2
Nitric oxide molecule
N    0.000000    0.000000    0.000000
O    0.000000    0.000000    1.151000`
    },
    ibuprofen: {
        name: "Ibuprofen",
        formula: "C₁₃H₁₈O₂",
        xyz: `33
Ibuprofen molecule
C    0.000000    0.000000    0.000000
C    1.396000    0.000000    0.000000
C    2.094000    1.209000    0.000000
C    1.396000    2.418000    0.000000
C    0.000000    2.418000    0.000000
C   -0.698000    1.209000    0.000000
C   -0.698000   -1.396000    0.000000
C   -2.094000   -1.396000    0.000000
C   -2.792000   -2.605000    0.000000
O   -4.018000   -2.605000    0.000000
O   -2.268000   -3.548000    0.000000
C    3.490000    1.209000    0.000000
C    4.188000    2.418000    0.000000
H    1.943000   -0.943000    0.000000
H    1.943000    3.361000    0.000000
H   -0.547000    3.361000    0.000000
H   -1.787000    1.209000    0.000000
H   -0.174000   -2.339000    0.000000
H   -2.618000   -0.452000    0.000000
H   -1.570000   -0.452000    0.000000
H    3.966000    0.265000    0.000000
H    3.966000    2.153000    0.000000
H    3.714000    3.362000    0.000000
H    4.662000    2.418000    0.890000
H    4.662000    2.418000   -0.890000
H   -4.542000   -3.548000    0.000000
C    0.698000   -2.605000    0.000000
C    1.396000   -3.814000    0.000000
H    0.174000   -2.605000    0.890000
H    0.174000   -2.605000   -0.890000
H    1.920000   -3.814000    0.890000
H    1.920000   -3.814000   -0.890000
H    0.872000   -4.757000    0.000000`
    },
    paracetamol: {
        name: "Paracetamol",
        formula: "C₈H₉NO₂",
        xyz: `20
Paracetamol molecule
C    0.000000    0.000000    0.000000
C    1.396000    0.000000    0.000000
C    2.094000    1.209000    0.000000
C    1.396000    2.418000    0.000000
C    0.000000    2.418000    0.000000
C   -0.698000    1.209000    0.000000
O   -0.698000   -1.209000    0.000000
N    3.490000    1.209000    0.000000
C    4.188000    2.418000    0.000000
O    5.414000    2.418000    0.000000
C    3.490000   -0.187000    0.000000
H    1.943000   -0.943000    0.000000
H    1.943000    3.361000    0.000000
H   -0.547000    3.361000    0.000000
H   -1.787000    1.209000    0.000000
H   -1.658000   -1.209000    0.000000
H    4.014000    0.756000    0.000000
H    3.966000   -0.131000    0.890000
H    3.966000   -0.131000   -0.890000
H    3.714000    3.362000    0.000000`
    }
}; 