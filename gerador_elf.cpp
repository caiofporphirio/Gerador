/*
------------------GERADOR ELF---------------------
Especificações:
Trabalho realizado em C++ com uso de C++11 no Linux Ubunto 14.04 LTS
Para rodar em linux: 

g++ -std=c++11 -Wall gerador_elf.cpp -o gerador_elf
./gerador_elf seuarquivo
chmod +x ./saida
./saida

Grupo:
Caio de Freitas Porphirio            11/0026012
Pedro Henrique Moreira Santana       11/0136080  
*/


#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <string.h>
#include <elfio/elfio.hpp>

void criaElf(char bssfrom[], char datafrom[], char textfrom[],int bssTam, int dataTam, int textTam, int primeirobss, int primeirodata);

int main(int argc, char *argv[]) {
	using namespace std;
	string ia32 = "";
	string arq_ia32 = argv[1];
	arq_ia32 += ".asm";
	string compilastring = "nasm -f elf -o tempObj.o ";
	compilastring += arq_ia32;	
	char compila[compilastring.size()+1];
	strcpy (compila, compilastring.c_str());
	system(compila);
	system("ld -m elf_i386 -o tempObj tempObj.o");
	system("objdump -h -D -M intel tempObj > tempDump.txt");
	bool textbegin = false, linhadotext = false, textend = false;
	bool databegin = false, linhadodata = false, dataend = false;
	vector<string>textOpcodes;
	vector<string>dataOpcodes;
	vector<string>bssOpcodes;
	vector<string>dataAddress;
	int tamanhoData = 0, tamanhoBss = 0, primeirodata = 0, primeirobss = 0;

	string tempDump = "tempDump.txt";
	ifstream file_a(tempDump);
	if (file_a.is_open()) {
		while (!file_a.eof()) {
			
			string linestring;
			while(getline(file_a, linestring )){
				stringstream linestream1, linestream2, datasizestream, bsssizestream;
				string first, opcodestr, dataAdd, strdatasize, strbsssize, primeirodatastr, primeirobssstr;

				linhadotext = false;
				linhadodata = false;
				if (linestring.find(".data ") != string::npos){
					datasizestream << linestring;
					datasizestream >> strdatasize;
					datasizestream >> strdatasize;
					datasizestream >> strdatasize;
					datasizestream >> primeirodatastr;
					tamanhoData = stoul(strdatasize, nullptr, 16);
					primeirodata = stoul(primeirodatastr, nullptr, 16);
				} 
				if (linestring.find(".bss ") != string::npos){
					bsssizestream << linestring;
					bsssizestream >> strbsssize;
					bsssizestream >> strbsssize;
					bsssizestream >> strbsssize;
					bsssizestream >> primeirobssstr;
					tamanhoBss = stoul(strbsssize, nullptr, 16);
					primeirobss = stoul(primeirobssstr, nullptr, 16);
				}

				linestring = linestring.substr(0,30);
				if (linestring.find(".text:") != string::npos){
					linestring = ".text:";
					textbegin = true;
					linhadotext = true;
				}
				if (linestring.find(".data:") != string::npos){
					linestring = ".data:";
					textend = true;
					databegin = true;
					linhadodata = true;
				}
				if (linestring.find(".bss:") != string::npos){
					linestring = ".bss:";
					dataend = true;
					textend	 = true;
				}
				if (linestring.find("...") != string::npos){
					linestring = "";
				}

				if (linestring.size() != 0){
					linestream1 << linestring;
					linestream2 << linestring;
					linestream1>>first;

					if(first.find(":") != string::npos){
						if(textbegin == true && textend == false && linhadotext == false){
							while(linestream1 >> opcodestr){
								textOpcodes.push_back(opcodestr);
							}

						}
						if(databegin == true && dataend == false && linhadodata == false){
							linestream2 >> dataAdd;
							dataAddress.push_back(dataAdd);
							while(linestream1 >> opcodestr){
								dataOpcodes.push_back(opcodestr);
							}
						}
					}else{
						if(databegin == true && dataend == false && linhadodata == false){
							int atual, diferenca, remover;
								
							atual = stoul(first, nullptr, 16);
							if(atual != primeirodata){
								
								diferenca = atual - primeirodata;
								remover = dataOpcodes.size() - diferenca;
								if(remover >= 0){
									for (int i=0;i <remover; i++){
										dataOpcodes.pop_back();
									}
								}else{
									string addzero = "00";
										for(int i=0; i>remover; i--){
											dataOpcodes.push_back(addzero);
									}
									
								}
							}
						}
					}

				}
				
			}

		}
		remove("tempDump.txt");
		remove("tempObj.o");
		remove("tempObj");
	}else {
		cout << "Não foi possível ler o arquivo!" << endl;
	}
	int bssOpcodessize = bssOpcodes.size();
	if(tamanhoBss > bssOpcodessize){
		string addzero = "00";
		int completa = tamanhoBss - bssOpcodes.size();
		for(int i=0; i<completa; i++){
			bssOpcodes.push_back(addzero);	
		}						
	}
	char bss[bssOpcodes.size()];
	for (unsigned int i = 0; i < bssOpcodes.size(); i++)
	{
		string auxHexa, auxCompleto;
		auxHexa= "0x";
		auxCompleto = auxHexa + bssOpcodes[i];
		int inteiro = stoul(auxCompleto, nullptr, 16);
		bss[i] = (char)inteiro;
	}
	int dataOpcodessize = dataOpcodes.size();
	if(tamanhoData > dataOpcodessize){
		string addzero = "00";
		int completa = tamanhoData - dataOpcodes.size();
		for(int i=0; i<completa; i++){
			dataOpcodes.push_back(addzero);	
		}						
	}
	char data[dataOpcodes.size()];
	for (unsigned int i = 0; i < dataOpcodes.size(); i++)
	{
		string auxHexa, auxCompleto;
		auxHexa= "0x";
		auxCompleto = auxHexa + dataOpcodes[i];
		int inteiro = stoul(auxCompleto, nullptr, 16);
		data[i] = (char)inteiro;
	}
	char text[textOpcodes.size()];
	for (unsigned int i = 0; i < textOpcodes.size(); i++)
	{
		string auxHexa, auxCompleto;
		auxHexa= "0x";
		auxCompleto = auxHexa + textOpcodes[i];
		int inteiro = stoul(auxCompleto, nullptr, 16);
		text[i] = (char)inteiro;
	}
	
    criaElf(bss, data, text, sizeof(bss), sizeof(data), sizeof(text),primeirobss, primeirodata);
	return 0;

}

using namespace ELFIO;
void criaElf(char bssfrom[], char datafrom[], char textfrom[],int bssTam, int dataTam, int textTam, int primeirobss, int primeirodata)
{
	char bss[bssTam];
	char data[dataTam];               
	char text[textTam];
	for(int i = 0; i < bssTam; i++){
    	bss[i] = bssfrom[i];
    }
	for(int i = 0; i < dataTam; i++){
    	data[i] = datafrom[i];
    }
    for(int i = 0; i < textTam; i++){
    	text[i] = textfrom[i];
    }
    
    elfio writer;
    
    // You can't proceed without this function call!
    writer.create( ELFCLASS32, ELFDATA2LSB );

    writer.set_os_abi( ELFOSABI_LINUX );
    writer.set_type( ET_EXEC );
    writer.set_machine( EM_386 );

    // Create code section
    section* text_sec = writer.sections.add( ".text" );
    text_sec->set_type( SHT_PROGBITS );
    text_sec->set_flags( SHF_ALLOC | SHF_EXECINSTR );
    text_sec->set_addr_align( 0x10 );
    

    text_sec->set_data( text, sizeof( text ) );
    
    // Create a loadable segment
    segment* text_seg = writer.segments.add();
    text_seg->set_type( PT_LOAD );
    text_seg->set_virtual_address( 0x08048080 );
    text_seg->set_physical_address( 0x08048080 );
    text_seg->set_flags( PF_X | PF_R );
    text_seg->set_align( 0x1000 );
    
    // Add code section into program segment
    text_seg->add_section_index( text_sec->get_index(), text_sec->get_addr_align() );
  
    if(primeirodata != 0){
	    // Create data section*
	    section* data_sec = writer.sections.add( ".data" );
	    data_sec->set_type( SHT_PROGBITS );
	    data_sec->set_flags( SHF_ALLOC | SHF_WRITE );
	    data_sec->set_addr_align( 0x4 );

	    
	    data_sec->set_data( data, sizeof( data ) );

	    // Create a read/write segment
	    segment* data_seg = writer.segments.add();
	    data_seg->set_type( PT_LOAD );
	    data_seg->set_virtual_address( primeirodata );
	    data_seg->set_physical_address( primeirodata );
	    data_seg->set_flags( PF_W | PF_R );
	    data_seg->set_align( 0x10 );

	    // Add code section into program segment
	    data_seg->add_section_index( data_sec->get_index(), data_sec->get_addr_align() );
	}

	if (primeirobss != 0){
		 // Create bss section*
	    section* bss_sec = writer.sections.add( ".bss" );
	    bss_sec->set_type( SHT_PROGBITS );
	    bss_sec->set_flags( SHF_ALLOC | SHF_WRITE );
	    bss_sec->set_addr_align( 0x4 );

	    
	    bss_sec->set_data( bss, sizeof( bss ) );

	    // Create a read/write segment
	    segment* bss_seg = writer.segments.add();
	    bss_seg->set_type( PT_LOAD );
	    bss_seg->set_virtual_address( primeirobss );
	    bss_seg->set_physical_address( primeirobss );
	    bss_seg->set_flags( PF_W | PF_R );
	    bss_seg->set_align( 0x10 );

	    // Add code section into program segment
	    bss_seg->add_section_index( bss_sec->get_index(), bss_sec->get_addr_align() );
	}

    // Setup entry point
    writer.set_entry( 0x08048080 );

    // Create ELF file
    writer.save( "saida" );

}
