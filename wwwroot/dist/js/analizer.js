function rgbToHex(r, g, b) {
  return '#' + ((1 << 24) | (r << 16) | (g << 8) | b).toString(16).slice(1);
}

function isValidHexColor(color) {
  return /^#[0-9A-F]{6}$/i.test(color);
}

// Функция для добавления пары dt-dd
function addInfoItem(container, label, value, id) {
  const item = document.createElement('div');
  item.className = 'flex justify-between text-xs border-b py-1';
  item.innerHTML = `
                <dt class="font-semibold">${label}</dt>
                <dd id="${id}" ${value == 0 ? 'style="color:#ff6e3e;"' : ''}>${value}</dd>
            `;
  container.appendChild(item);
}

function displayImageInfo(desc, file) {
  const infoContainer = document.getElementById('pngImageInfo');
  infoContainer.innerHTML = '';

  // Основная информация о файле
  addInfoItem(infoContainer, 'File:', file.name, '');
  addInfoItem(infoContainer, 'Size:', `${(file.size / 1024).toFixed(2)} KB`, '');
  addInfoItem(infoContainer, 'Dimensions:', `${desc.size.getW()} × ${desc.size.getH()}`, '');
  addInfoItem(infoContainer, 'Bit Depth:', desc.bitDepth, '');
  addInfoItem(infoContainer, 'Color Type:', desc.colorType, '');
  addInfoItem(infoContainer, 'Channels:', desc.channels, '');
  addInfoItem(infoContainer, 'BPP:', desc.bpp, '');
  addInfoItem(infoContainer, 'Data Length:', `${desc.buf.len} bytes`, '');
  addInfoItem(infoContainer, 'Palette Count:', desc.paletteCount, 'png_palette_count');
  addInfoItem(infoContainer, 'Colors Per Palette:', desc.colorsPerPalette, 'png_colors_per_palette');

  if (desc.paletteCount > 0 && desc.palette) {
    addInfoItem(infoContainer, 'Palette Colors:', desc.palette.colors.size(), '');
  }

  // Заголовок для анализа цветов.
  const colorAnalysisHeader = document.createElement('div');
  colorAnalysisHeader.className = 'flex justify-between text-xs font-bold mt-2';
  colorAnalysisHeader.innerHTML = '<dt>Color Analysis:</dt><dd></dd>';
  infoContainer.appendChild(colorAnalysisHeader);

  const colorAnalysis = desc.colorAnalysis;
  addInfoItem(infoContainer, 'Total Pixels:', colorAnalysis.totalPixels, '');
  addInfoItem(infoContainer, 'Unique Colors:', colorAnalysis.uniqueColors.size(), '');

  const uniqueColorList = document.getElementById('pngUniqueColors');
  uniqueColorList.innerHTML = '';

  if (colorAnalysis.uniqueColors.size() > 0) {
    for (let i = 0; i < colorAnalysis.uniqueColors.size(); i++) {
      const color = colorAnalysis.uniqueColors.get(i);
      // Пропускаем прозрачные цвета.
      if (color.getA() == 0) {
        continue;
      }

      const hexColor = rgbToHex(color.getR(), color.getG(), color.getB());

      const colorListItem = document.createElement('li');
      colorListItem.innerHTML = `
        <div class="color-box">
          <div class="color" style="background-color: ${hexColor};"></div>
          <input class="name selectable text-xs" type="text" readonly="readonly" value="${hexColor}">
        </div>
      `;

      uniqueColorList.appendChild(colorListItem);
    }
  }

  const colorList = document.getElementById('pngPaletteColors');
  colorList.innerHTML = '';

  if (desc.palette.colors.size() > 0) {
    for (let i = 0; i < desc.palette.colors.size(); i++) {
      const color = desc.palette.colors.get(i);
      // Пропускаем прозрачные цвета.
      // if (color.getA() == 0) {
      //   continue;
      // }

      const hexColor = rgbToHex(color.getR(), color.getG(), color.getB());

      const colorListItem = document.createElement('li');
      colorListItem.innerHTML = `
        <div class="color-box">
          <div class="color" style="background-color: ${hexColor};"></div>
          <input class="name selectable text-xs" type="text" readonly="readonly" value="${hexColor}">
        </div>
      `;

      colorList.appendChild(colorListItem);
    }
  }

  // infoContainer.style.display = 'block';
}
