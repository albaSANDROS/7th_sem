    @staticmethod
    def roberts_operator(image):
        im = image.copy()
        pixels = im.load()
        w, h = im.size

        for i in range(w - 1):
            for j in range(h - 1):
                pixels[i, j] = tuple(
                    map(lambda x1, x2, x3, x4: int(math.sqrt((x1 - x2) ** 2 + (x2 - x3) ** 2)), pixels[i + 1, j],
                        pixels[i, j + 1], pixels[i, j], pixels[i + 1, j + 1]))

        return im



    @staticmethod
    def logarithmic_correction(image):
        constant = input('Enter constant:')
        im = image.copy()
        pixels = im.load()

        for i in range(im.size[0]):
            for j in range(im.size[1]):
                pixels[i, j] = tuple(map(lambda x: int(constant * math.log(1 + x)), pixels[i, j]))
        return im
