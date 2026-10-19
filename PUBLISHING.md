# Publicação no GitHub

1. Crie um repositório público, por exemplo `fnaf2-dsi-xl`.
2. Copie seu código-fonte para `source/`, preservando Makefile e estrutura de assets permitidos.
3. Use o `README.md` desta pasta na raiz do repositório.
4. Faça uma auditoria de direitos autorais antes de incluir imagens, áudio e ROM contendo materiais de terceiros. A licença do código precisa ser escolhida separadamente.
5. Envie o código com Git (`git add .`, `git commit -m "Release V19.7"`, `git push`).
6. Em Settings > Pages, escolha Deploy from a branch, branch `main`, pasta `/docs`; para isso, renomeie a pasta `site` para `docs`.
7. Acesse o endereço do GitHub Pages indicado nas configurações.
8. Se a distribuição da ROM for permitida e a build estiver validada, crie uma Release com a tag `v19.7` e anexe o arquivo `.nds`.
9. Substitua os links genéricos do GitHub no site pelo endereço real do repositório.

O site não contém a ROM nem o código-fonte do jogo: são arquivos que você acrescentará.


### Vídeo e GitHub Pages

O arquivo de vídeo fica em `site/videos/` e pesa cerca de 33 MB. GitHub Pages pode servir esse MP4 como arquivo estático. Se publicar a página na pasta `site`, configure o GitHub Pages para servir essa pasta via workflow GitHub Actions ou mova o conteúdo de `site/` para `docs/` e selecione `/docs` nas configurações de Pages. Não é necessário backend. O navegador só começa a carregar o vídeo quando o usuário interage.
