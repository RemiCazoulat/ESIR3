def constructMatrixOfMonomials(g, order):
    print("Constructing matrix of monomials G...")
    G = []
    for k in range(0, len(g)):
        tmp_g = []
        for i in range(0, order + 1):
            for j in range(0, order - i + 1):
                tmp = (pow(g[k][0], i)) * (pow(g[k][1], j)) * (pow(g[k][2], (order - i - j)))
                tmp_g.append(tmp)
        G.append(tmp_g)
    print("Done")
    return G
