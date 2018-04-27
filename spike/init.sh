#  download and unzip data
mkdir -p data
python3 spike/dl_data.py

#  format data
python3 spike/trim_data.py
